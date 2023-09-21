#include "montealg.h"

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
    m_lost = MAX_SCH_WINX;
    memset(m_valid, 0, sizeof(m_valid));
}

// 重头开始
void MonteAlg::restart(SqlIn& in)
{
    Q_UNUSED(in);

    /* 清除状态 */
    memset(m_valid, 0, sizeof(m_valid));
    m_lost = MIN(m_lost << 1, MAX_SCH_WINX);
}

const FreqRsp& MonteAlg::bandit(SqlIn& in, const FreqReq& req)
{
    /* 分层聚类 */
    bool flag = kmean(in);

    FreqRsp* rsp = &m_rsp;
    int m = MIN(req.fcNum, RSP_FREQ_NUM);
    int fid = 0, glbChId, minGlbId, maxGlbId;

    /* 添加聚类 */
    if (flag == true) {
        /* 限制带宽 */
        minGlbId = m_kmList.first();
        maxGlbId = m_kmList.back();
        limit(minGlbId, maxGlbId);

        /* f0 Menta Carlo Search Tree */
        glbChId = m_tree[m_treeId];
        m_treeId = (m_treeId + 1) & MAX_TREE_MSK;
        rsp->glb[fid++] = align(glbChId);

        if (m_lost <= 1) {
            /* f1:k0随机 */
            int k0 = m_kmList.at(0);
            int i = qrand() % FST_RND_RNG;
            int j = i - FST_RND_RNG / 2;
            glbChId = align(k0 + j);
            rsp->glb[fid++] = glbChId;
            m_valid[k] = true;

            /* f2:第2类 */
            int n = m_kmList.size();
            if (n > 1) {
                rsp->glb[fid++] = m_kmList.at(1);
            }

            /* f3:第1类k0 */
            rsp->glb[fid++] = k0;
        }
    } else {
        glbChId = initChId();
        m_valid[glbChId] = true;
        maxGlbId = MAX_GLB_CHN;
        minGlbId = 0;
    }

    /* 补充二分推荐 */
    while (fid < m) {
        if (bisect(minGlbId, maxGlbId, glbChId)) {
            rsp->glb[fid++] = align(glbChId);
        }
    }

    set_head(fid);
    return m_rsp;
}

// 选择历史样本点分层聚类
bool MonteAlg::kmean(SqlIn& in)
{
    /* 读取记录 */
    const Time* ts = in.stamp;
    in.mysql->select(SMPL_LINK, ts, in.myRule, m_sqlList);
    if (m_sqlList.isEmpty()) {
        return false;
    }

    /* 样本清零 */
    m_kmean->clear();
    int n = m_sqlList.size();
    int i, k;

    /* case1:30min */
    int minMin, minHr;
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
    if (out.isValid == true) {
        m_lost = MAX(m_lost >> 1, 1);
        memset(m_valid, 0, sizeof(m_valid));
    }

    /* 能效评估 */
    BaseAlg::notify(in, glbChId, out);
    return m_regret;
}

// 带宽限制
void MonteAlg::limit(int& minGlbId, int& maxGlbId)
{
    int sqlMin = minGlbId;
    int sqlMax = maxGlbId;
    int schband = m_lost * BASIC_SCH_WIN;
    int schWin = schband / ONE_CHN_BW;
    int halfWin = (schWin >> 1);
    minGlbId = MAX(sqlMin - halfWin, 0);
    maxGlbId = MIN(MAX(sqlMax + halfWin, minGlbId + schWin), MAX_GLB_CHN);
}

void MonteAlg::tree(int minGlbId, int maxGlbId)
{
    /* 初始化边界 */
    memset(m_flag, 0, sizeof(m_flag));
    m_flag[maxGlbId] = true;
    m_flag[minGlbId] = true;

    int fid = 0;
    while (fid < MAX_TREE_LEN) {
        int maxLen = 0;
        int start = minGlbId;
        int stop = maxGlbId;

        /* 正向 */
        int i, j, k;
        for (i = minGlbId, j = minGlbId + 1; j <= maxGlbId; j++) {
            if (m_flag[j] == true) {
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
            m_tree[fid++] = start + qrand() % half + quart;
        } else {
            m_tree[fid++] = (start + stop) >> 1;
        }
    }

    /* 索引复位 */
    m_treeId = 0;
}

