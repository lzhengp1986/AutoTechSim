#include "montealg.h"
#include "sql/beta.h"

MonteAlg::MonteAlg(void)
{
    reset();
    m_kmean = new KMean;
}

MonteAlg::~MonteAlg(void)
{
    delete m_kmean;
    m_kmean = nullptr;
}

void MonteAlg::reset(void)
{
    BaseAlg::reset();

    /* 搜索树 */
    tree(0, MAX_GLB_CHN);

    /* 清除状态 */
    m_stage = MAX_SCH_WINX;
    memset(m_valid, 0, sizeof(m_valid));
}

// 重头开始
void MonteAlg::restart(SqlIn& in)
{
    Q_UNUSED(in);

    /* 清除状态 */
    memset(m_valid, 0, sizeof(m_valid));
    m_stage = MIN(m_stage << 1, MAX_SCH_WINX);
}

const FreqRsp& MonteAlg::bandit(SqlIn& in, const FreqReq& req)
{
    /* 1.分层聚类 */
    bool flag = kmean(in, m_stage);

    FreqRsp* rsp = &m_rsp;
    int m = MIN(req.fcNum, RSP_FREQ_NUM);
    int fid = 0, minGlbId, maxGlbId;

    /* 2.f0:MentaCarlo */
    int glbChId = thomp();
    int i = qrand() % FST_RND_RNG;
    int j = i - FST_RND_RNG / 2;
    int k0 = MAX(glbChId + j, 0);
    rsp->glb[fid++] = align(k0);
    m_valid[glbChId] = true;

    /* 3.聚类推荐 */
    if (flag == true) {
        if (m_stage <= 1) {
            /* f1:k0随机 */
            k0 = m_kmList.at(0);
            i = qrand() % FST_RND_RNG;
            j = i - FST_RND_RNG / 2;
            glbChId = MAX(k0 + j, 0);
            rsp->glb[fid++] = align(glbChId);
            m_valid[glbChId] = true;

            /* f2:第2类 */
            if (m_kmList.size() > 1) {
                glbChId = m_kmList.at(1);
                rsp->glb[fid++] = glbChId;
                m_valid[glbChId] = true;
            }

            /* f3:第1类k0 */
            rsp->glb[fid++] = k0;
            m_valid[k0] = true;
        }

        /* 限制带宽 */
        int sqlMin = m_kmList.first();
        int sqlMax = m_kmList.back();
        int schband = m_stage * BASIC_SCH_WIN;
        int schWin = schband / ONE_CHN_BW;
        int halfWin = (schWin >> 1);
        minGlbId = MAX(sqlMin - halfWin, 0);
        maxGlbId = MIN(MAX(sqlMax + halfWin, minGlbId + schWin), MAX_GLB_CHN);
    } else {
        maxGlbId = MAX_GLB_CHN;
        minGlbId = 0;
    }

    /* 4.补充二分推荐 */
    while (fid < m) {
        if (bisect(minGlbId, maxGlbId, glbChId)) {
            rsp->glb[fid++] = align(glbChId);
        }
    }

    set_head(fid);
    return m_rsp;
}

// 选择历史样本点分层聚类
bool MonteAlg::kmean(SqlIn& in, int stage)
{
    /* 读取记录 */
    m_sqlList.clear();
    const Time* ts = in.stamp;
    in.mysql->select(SMPL_LINK, ts, in.myRule, m_sqlList);
    if (m_sqlList.isEmpty()) {
        return false;
    }

    /* 样本清零 */
    m_kmean->clear();
    int n = m_sqlList.size();
    int i, k;

    int minMin, minHr;
    if (stage <= 1) {
        /* case1:30min */
        if (ts->min > 30) {
            /* 当前小时 */
            minHr = ts->hour;
            minMin = ts->min - 30;
            for (i = 0; i < n; i++) {
                FreqInfo& info = m_sqlList[i];
                if ((info.hour == minHr) && (info.min >= minMin)) {
                    m_valid[info.glbChId] = true;
                    m_kmean->push(info);
                    info.isNew = false;
                }
            }
        } else {
            /* 当前小时+前1小时 */
            k = ts->hour;
            minHr = (k + MAX_HOUR_NUM - 1) % MAX_HOUR_NUM;
            minMin = (ts->min + 30) % 60;
            for (i = 0; i < n; i++) {
                FreqInfo& info = m_sqlList[i];
                if (((info.hour == minHr) && (info.min >= minMin))
                    || (info.hour == k)) {
                    m_valid[info.glbChId] = true;
                    m_kmean->push(info);
                    info.isNew = false;
                }
            }
        }

        /* case1样本聚类 */
        int n30m = m_kmean->sche(m_kmList);
        if (n30m > 0) {
            return true;
        }
    }

    if (stage <= 2) {
        /* case2: 1hour */
        k = ts->hour;
        minMin = ts->min;
        minHr = (k + MAX_HOUR_NUM - 1) % MAX_HOUR_NUM;
        for (i = 0; i < n; i++) {
            FreqInfo& info = m_sqlList[i];
            if ((((info.hour == minHr) && (info.min >= minMin))
                || (info.hour == k)) && (info.isNew == true)) {
                m_valid[info.glbChId] = true;
                m_kmean->push(info);
                info.isNew = false;
            }
        }

        /* case2样本聚类 */
        int n1h = m_kmean->sche(m_kmList);
        if (n1h > 0) {
            return true;
        }
    }

    /* case3: 所有样本 */
    for (i = 0; i < n; i++) {
        const FreqInfo& info = m_sqlList[i];
        if (info.isNew == true) {
            m_valid[info.glbChId] = true;
            m_kmean->push(info);
        }
    }

    /* case2样本聚类 */
    k = m_kmean->sche(m_kmList);
    return (k > 0);
}

bool MonteAlg::bisect(int minGlbId, int maxGlbId, int& glbChId)
{
    /* 二分搜索 */
    int maxLen = 0;
    int start = minGlbId;
    int stop = maxGlbId;
    m_valid[minGlbId] = true;
    m_valid[maxGlbId] = true;

    /* 正向 */
    int i, j, k;
    for (i = minGlbId, j = minGlbId + 1; j <= maxGlbId; j++) {
        if (m_valid[j] == true) {
            k = j - i - 1;
            if (k > maxLen) {
                maxLen = k;
                start = i;
                stop = j;
            }
            i = j;
        }
    }

    /* 无可用频率 */
    if (maxLen <= 1) {
        return false;
    }

    /* 二分位 */
    if (maxLen > 4) {
        int half = maxLen >> 1;
        int quart = half >> 1;
        glbChId = start + qrand() % half + quart;
    } else {
        glbChId = (start + stop) >> 1;
    }

    m_valid[glbChId] = true;
    return true;
}

int MonteAlg::notify(SqlIn& in, int glbChId, const EnvOut& out)
{
    if (glbChId >= MAX_GLB_CHN) {
        return m_regret;
    }

    /* 状态切换 */
    bool flag = out.isValid;
    if (flag == true) {
        m_stage = MAX(m_stage >> 1, 1);
        memset(m_valid, 0, sizeof(m_valid));
    }

    /* thompson统计 */
    thomp(glbChId, flag);

    /* 能效评估 */
    BaseAlg::notify(in, glbChId, out);
    return m_regret;
}

void MonteAlg::tree(int minGlbId, int maxGlbId)
{
    /* 初始化边界+随机 */
    static bool used[MAX_GLB_CHN] = {0};
    int rnd = qrand() % MAX_GLB_CHN;
    used[maxGlbId] = true;
    used[minGlbId] = true;
    used[rnd] = true;

    int fid = 0;
    while (fid < MAX_TREE_LEN) {
        int maxLen = 0;
        int start = minGlbId;
        int stop = maxGlbId;

        /* 正向 */
        int i, j, k;
        for (i = minGlbId, j = minGlbId + 1; j <= maxGlbId; j++) {
            if (used[j] == true) {
                k = j - i - 1;
                if (k > maxLen) {
                    maxLen = k;
                    start = i;
                    stop = j;
                }
                i = j;
            }
        }

        /* 二分位 */
        if (maxLen > 4) {
            int half = maxLen >> 1;
            int quart = half >> 1;
            k = start + qrand() % half + quart;
        } else {
            k = (start + stop) >> 1;
        }

        /* 保存信道号 */
        m_tree[fid] = k;
        used[k] = true;

        /* 统计初始化 */
        m_vldNum[fid] = 10;
        m_invNum[fid] = 1;
        fid++;
    }
}

// thompson统计
void MonteAlg::thomp(int glbChId, bool flag)
{
    /* 找最近信道号 */
    int i, j, k = 0;
    int md = ABS(glbChId - m_tree[k]);
    for (i = 1; i < MAX_TREE_LEN; i++) {
        j = ABS(glbChId - m_tree[i]);
        if (j < md) {
            md = j;
            k = i;
        }
    }

    /* 结果统计 */
    m_vldNum[k] += (flag == true);
    m_invNum[k] += (flag == false);
}

// thompson推荐
int MonteAlg::thomp(void)
{
    int k = 0;
    double pi;

    /* 取最大概率对应信道 */
    int rnd = qrand();
    double recp = 1.0 / RAND_MAX;
    double px = (double)(rnd + (!rnd)) * recp;
    double pm = beta(m_vldNum[k] + 1, m_invNum[k] + 1, px);
    for (int i = 1; i < MAX_TREE_LEN; i++) {
        rnd = qrand();
        px = (double)(rnd + (!rnd)) * recp;
        pi = beta(m_vldNum[i] + 1, m_invNum[i] + 1, px);
        if (pi > pm) {
            pm = pi;
            k = i;
        }
    }

    return m_tree[k];
}
