#include "montealg.h"

MonteAlg::MonteAlg(void)
{
    m_kmean = new KMean;
    reset();
}

MonteAlg::~MonteAlg(void)
{
    delete m_kmean;
    m_kmean = nullptr;
}

void MonteAlg::reset(void)
{
    BaseAlg::reset();
    memset(m_valid, 0, sizeof(m_valid));
    m_lost = true;
}

// 重新开始
void MonteAlg::restart(SqlIn& in)
{
    memset(m_valid, 0, sizeof(m_valid));
    m_lost = true;
}

const FreqRsp& MonteAlg::bandit(SqlIn& in, const FreqReq& req)
{
    Q_UNUSED(in);
    int i, j, k;

    /* 读取记录 */
    in.mysql->select(SMPL_LINK, in.stamp, in.myRule, m_sqlList);
    int n = m_sqlList.size();

    /* 聚类 */
    if (n > 0) {
        for (i = 0; i < n; i++) {
            m_kmean->push(m_sqlList.at(i));
        }
        m_kmean->kmean(m_kmList);
    } else {
        m_kmList.clear();
        int mid = middle();
        m_kmList.append(mid);
    }

    /* 搜索带宽 */
    int schband;
    if (m_lost == true) {
        int rnd = qrand() % 100;
        if (rnd < 40) { /* 40% */
            schband = BASIC_SCH_WIN * 2;
        } else if (rnd < 60) { /* 20% */
            schband = BASIC_SCH_WIN * 4;
        } else if (rnd < 80) { /* 20% */
            schband = BASIC_SCH_WIN * 6;
        } else { /* 20% */
            schband = BASIC_SCH_WIN * 8;
        }
    } else {
        schband = BASIC_SCH_WIN;
    }
    int schWin = schband / ONE_CHN_BW;

    /* 添加WIN内结果 */
    /* 信道不足则二分 */

    set_head(j);
    return m_rsp;
}

int MonteAlg::notify(SqlIn& in, int glbChId, const EnvOut& out)
{
    if (glbChId >= MAX_GLB_CHN) {
        return m_regret;
    }

    /* 状态切换 */
    m_lost = false;

    /* 能效评估 */
    BaseAlg::notify(in, glbChId, out);
    return m_regret;
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
