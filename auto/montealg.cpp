#include "montealg.h"
#include <QRandomGenerator>

MonteAlg::MonteAlg(void)
    : m_bisect(new Bisecting)
    , m_cluster(new KBeta)
    , m_beta(new RandGen(1987))
{
    reset();
}

MonteAlg::~MonteAlg(void)
{
    delete m_beta;
    delete m_bisect;
    delete m_cluster;
    m_cluster = nullptr;
    m_bisect = nullptr;
    m_beta = nullptr;
}

void MonteAlg::reset(void)
{
    BaseAlg::reset();

    /* 初始中心 */
    m_prvGlbChId = initChId();

    /* 搜索树 */
    tree(0, MAX_GLB_CHN - 1);

    /* 清除状态 */
    m_stage = MAX_STAGE_NUM;
    m_bisect->clear();
}

// 重头开始
void MonteAlg::restart(SqlIn& in, unsigned& failNum)
{
    Q_UNUSED(in);
    if (failNum >= m_stage) {
        m_stage = MIN(m_stage << 1, MAX_STAGE_NUM);
        m_bisect->clear();
        failNum = 0;
    }
}

const FreqRsp& MonteAlg::bandit(SqlIn& in, const FreqReq& req)
{
    /* 1.分层聚类 */
    bool flag = kmean(in, m_stage);

    /* 2.计算搜索带宽 */
    int schRng = m_stage * BASIC_SCH_WIN;
    int schWin = schRng / ONE_CHN_BW;
    int halfWin = (schWin >> 1);

    /* 3.聚类推荐 */
    FreqRsp* rsp = &m_rsp;
    int minGlbId, maxGlbId;
    int f0, f1, f2, fid = 0;

    if (flag == true) {
        if (m_stage <= (MAX_STAGE_NUM >> 1)) {
            /* f0:第1类 */
            int f0 = m_kmList.at(0);

            /* f1:第2类或f0随机 */
            if (m_kmList.size() > 1) {
                f1 = m_kmList.at(1);
            } else {
                f1 = chId300K(f0);
            }

            /* f2.MentaCarlo */
            f2 = thomp();
            f2 = chId300K(f2);

            rsp->glb[fid++] = f0;
            rsp->glb[fid++] = f1;
            rsp->glb[fid++] = f2;
            m_bisect->setValid(f0);
            m_bisect->setValid(f1);
            m_bisect->setValid(f2);
        }

        /* 限制带宽 */
        int sqlMin = m_kmList.first();
        int sqlMax = m_kmList.back();
        minGlbId = MAX(sqlMin - halfWin, 0);
        maxGlbId = MIN(MAX(sqlMax + halfWin, minGlbId + schWin), MAX_GLB_CHN - 1);
    } else {
        minGlbId = MAX(m_prvGlbChId - halfWin, 0);
        maxGlbId = MIN(minGlbId + schWin, MAX_GLB_CHN - 1);

        /* f0:MentaCarlo */
        f0 = thomp();
        f0 = chId300K(f0);
        rsp->glb[fid++] = f0;
        m_bisect->setValid(f0);
    }

    /* 4.补充二分推荐 */
    int m = MIN(req.fcNum, RSP_FREQ_NUM);
    while (fid < m) {
        if (m_bisect->sche(minGlbId, maxGlbId, f0)) {
            rsp->glb[fid++] = f0;
        }
    }

    set_head(fid);
    return m_rsp;
}

// 选择历史样本点分层聚类
bool MonteAlg::kmean(SqlIn& in, int stage)
{
    /* 读取记录 */
    const Time* ts = in.stamp;
    in.mysql->select(SMPL_LINK, ts, in.myRule, m_sqlList);
    if (m_sqlList.isEmpty()) {
        return false;
    }

    /* 样本清零 */
    m_cluster->clear();
    int n = m_sqlList.size();

    int i, maxMin, maxHr;
    int minMin = ts->min;
    int minHr = ts->hour;
    if (stage >= (MAX_STAGE_NUM >> 1)) {
        /* case1:30min */
        if (minMin < 30) {
            /* 当前小时 */
            maxMin = minMin + 30;
            for (int i = 0; i < n; i++) {
                FreqInfo& info = m_sqlList[i];
                if (info.isNew == false) {
                    continue;
                }
                if ((info.hour == minHr)
                    && (info.min >= minMin)
                    && (info.min <= maxMin)) {
                    m_cluster->push(info);
                    info.isNew = false;
                }
            }
        } else {
            /* 当前小时+后1小时 */
            maxMin = (minMin + 30) % 60;
            maxHr = (minHr + 1) % MAX_HOUR_NUM;
            for (int i = 0; i < n; i++) {
                FreqInfo& info = m_sqlList[i];
                if (info.isNew == false) {
                    continue;
                }
                if (((info.hour == minHr) && (info.min >= minMin))
                    || ((info.hour == maxHr) && (info.min <= maxMin))) {
                    m_cluster->push(info);
                    info.isNew = false;
                }
            }
        }

        /* case1样本聚类 */
        int n30m = m_cluster->sche(m_kmList);
        if (n30m > 0) {
            return true;
        }
    }

    if (stage >= (MAX_STAGE_NUM >> 2)) {
        /* case2: 1hour */
        maxHr = (minHr + 1) % MAX_HOUR_NUM;
        for (i = 0; i < n; i++) {
            FreqInfo& info = m_sqlList[i];
            if (info.isNew == false) {
                continue;
            }
            if (((info.hour == minHr) && (info.min >= minMin))
                || ((info.hour == maxHr) && (info.min <= minMin))) {
                m_cluster->push(info);
                info.isNew = false;
            }
        }

        /* case2样本聚类 */
        int n1h = m_cluster->sche(m_kmList);
        if (n1h > 0) {
            return true;
        }
    }

    /* case3: 所有样本 */
    for (i = 0; i < n; i++) {
        const FreqInfo& info = m_sqlList[i];
        if (info.isNew == false) {
            continue;
        }
        m_cluster->push(info);
    }

    /* case2样本聚类 */
    int k = m_cluster->sche(m_kmList);
    return (k > 0);
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
        m_prvGlbChId = glbChId;
        m_bisect->clear();
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
    bool used[MAX_GLB_CHN + 1] = {0};
    UnifIntDist dist(0, MAX_GLB_CHN - 1);
    int rnd = dist(*m_gen[RSV]);
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
            UnifIntDist dist(0, half);
            rnd = dist(*m_gen[RSV]);
            k = start + rnd + quart;
        } else {
            k = (start + stop) >> 1;
        }

        /* 保存信道号 */
        m_tree[fid] = k;
        used[k] = true;

        /* 统计初始化 */
        m_vldNum[fid] = 0;
        m_invNum[fid] = 0;
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
    if (flag == true) {
        m_vldNum[k]++;
    } else {
        m_invNum[k]++;
    }
}

// thompson推荐
int MonteAlg::thomp(void)
{
    int k = 0;
    /* 取最大概率对应信道 */
    BetaDist beta(m_vldNum[k] + 1, m_invNum[k] + 1);
    double pi, pm = beta(*m_beta);
    for (int i = 1; i < MAX_TREE_LEN; i++) {
        BetaDist beta(m_vldNum[i] + 1, m_invNum[i] + 1);
        pi = beta(*m_beta);
        if (pi > pm) {
            pm = pi;
            k = i;
        }
    }

    return m_tree[k];
}
