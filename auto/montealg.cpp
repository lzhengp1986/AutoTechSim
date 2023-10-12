#include "montealg.h"
#include <QRandomGenerator>

MonteAlg::MonteAlg(void)
{
    reset();
    m_cluster = new KBeta;
}

MonteAlg::~MonteAlg(void)
{
    delete m_cluster;
    m_cluster = nullptr;
}

void MonteAlg::reset(void)
{
    BaseAlg::reset();

    /* 初始中心 */
    m_prvGlbChId = initChId();

    /* 搜索树 */
    tree(0, MAX_GLB_CHN - 1);

    /* 清除状态 */
    m_stage = MAX_SCH_WINX;
    memset(m_valid, 0, sizeof(m_valid));
}

// 重头开始
void MonteAlg::restart(SqlIn& in, unsigned& failNum)
{
    Q_UNUSED(in);
    if (failNum >= OPT_RESTART_THR) {
        memset(m_valid, 0, sizeof(m_valid));
        m_stage = MIN(m_stage << 1, MAX_SCH_WINX);
        failNum = 0;
    }
}

const FreqRsp& MonteAlg::bandit(SqlIn& in, const FreqReq& req)
{
    /* 1.分层聚类 */
    bool flag = kmean(in, m_stage);

    int glbChId, fid = 0;
    int minGlbId, maxGlbId;

    /* 2.计算搜索带宽 */
    int schband = m_stage * OPT_SCH_WIN;
    int schWin = schband / ONE_CHN_BW;
    int halfWin = (schWin >> 1);

    /* 3.聚类推荐 */
    FreqRsp* rsp = &m_rsp;
    if (flag == true) {
        if (m_stage <= (MAX_SCH_WINX >> 1)) {
            /* f0:第1类 */
            int f0 = m_kmList.at(0);
            rsp->glb[fid++] = f0;
            m_valid[f0] = true;

            /* f1:第2类 */
            if (m_kmList.size() > 1) {
                glbChId = m_kmList.at(1);
                rsp->glb[fid++] = glbChId;
                m_valid[glbChId] = true;
            }

            /* f2:f0随机 */
            glbChId = chId300K(f0);
            rsp->glb[fid++] = glbChId;
            m_valid[glbChId] = true;
        }

        /* 限制带宽 */
        int sqlMin = m_kmList.first();
        int sqlMax = m_kmList.back();
        minGlbId = MAX(sqlMin - halfWin, 0);
        maxGlbId = MIN(MAX(sqlMax + halfWin, minGlbId + schWin), MAX_GLB_CHN - 1);
    } else {
        minGlbId = MAX(m_prvGlbChId - halfWin, 0);
        maxGlbId = MIN(minGlbId + schWin, MAX_GLB_CHN - 1);
    }

    /* 4.f3:MentaCarlo */
    int f3 = thomp();
    glbChId = chId300K(f3);
    rsp->glb[fid++] = glbChId;
    m_valid[glbChId] = true;

    /* 5.补充二分推荐 */
    int m = MIN(req.fcNum, RSP_FREQ_NUM);
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
    if (stage >= (MAX_SCH_WINX >> 1)) {
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

    if (stage >= (MAX_SCH_WINX >> 2)) {
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
        int r = rab1(0, half, &m_seedi);
        glbChId = start + r + quart;
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
        m_prvGlbChId = glbChId;
    }

    /* thompson统计 */
    thomp(glbChId, flag);

    /* 能效评估 */
    BaseAlg::notify(in, glbChId, out);
    return m_regret;
}

void MonteAlg::tree(int minGlbId, int maxGlbId)
{
    /* 质数表 */
    static int prime[500] = {
        #include "auto/prime.txt"
    };

    /* 初始化边界+随机 */
    static bool used[MAX_GLB_CHN + 1] = {0};
    int rnd = rab1(0, MAX_GLB_CHN - 1, &m_seedi);
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
            int r = rab1(0, half, &m_seedi);
            k = start + r + quart;
        } else {
            k = (start + stop) >> 1;
        }

        /* 保存信道号 */
        m_tree[fid] = k;
        used[k] = true;

        /* 统计初始化 */
        i = QRandomGenerator::global()->bounded(0, (int)MAX_TREE_LEN - 1);
        m_seed[fid] = prime[fid];
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
    m_vldNum[k] += (flag == true);
    m_invNum[k] += (flag == false);
}

// thompson推荐
int MonteAlg::thomp(void)
{
    int k = 0;
    double pi;

    /* 取最大概率对应信道 */
    double pm = rbeta(m_vldNum[k] + 1, m_invNum[k] + 1, m_seed);
    for (int i = 1; i < MAX_TREE_LEN; i++) {
        pi = rbeta(m_vldNum[i] + 1, m_invNum[i] + 1, m_seed + i);
        if (pi > pm) {
            pm = pi;
            k = i;
        }
    }

    return m_tree[k];
}
