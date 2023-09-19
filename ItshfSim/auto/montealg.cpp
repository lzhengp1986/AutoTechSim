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
    m_lost = MAX_SCH_WINX;
    memset(m_valid, 0, sizeof(m_valid));
}

// 重头开始
void MonteAlg::restart(SqlIn& in)
{
    Q_UNUSED(in);
    memset(m_valid, 0, sizeof(m_valid));
    m_lost = MIN(m_lost + 1, MAX_SCH_WINX);
}

const FreqRsp& MonteAlg::bandit(SqlIn& in, const FreqReq& req)
{
    /* 读取记录 */
    in.mysql->select(SMPL_LINK, in.stamp, in.myRule, m_sqlList);
    int n = m_sqlList.size();

    int i, j, k;
    if (n > 0) {
        /* 样本输入 */
        for (i = 0; i < n; i++) {
            const FreqInfo& info = m_sqlList.at(i);
            m_valid[info.glbChId] = true;
            m_kmean->push(info);
        }

        /* 样本聚类 */
        m_kmean->sche(m_kmList);
    } else {
        i = middle();
        m_kmList.clear();
        m_kmList.append(i);
        m_valid[i] = true;
    }

    int schband = m_lost * BASIC_SCH_WIN;
    int schWin = schband / ONE_CHN_BW;
    int halfWin = (schWin >> 1);

    /* 以k0为中心限带宽 */
    int k0 = m_kmList.at(0);
    int minGlbId = MAX(k0 - halfWin, 0);
    int maxGlbId = MIN(minGlbId + schWin, MAX_GLB_CHN);

    /* 添加随机频率 */
    FreqRsp* rsp = &m_rsp;
    i = qrand() % FST_RND_RNG;
    j = i - FST_RND_RNG / 2;
    rsp->glb[0] = k0 + j;
    rsp->glb[1] = k0;

    /* 添加WIN内结果 */
    n = m_kmList.size();
    int m = MIN(req.fcNum, RSP_FREQ_NUM);
    for (i = 1, j = 2; i < n; i++) {
        k = m_kmList.at(i);
        if ((k >= minGlbId) && (k <= maxGlbId)) {
            rsp->glb[j++] = k;
            if (j >= m) {
                break;
            }
        }
    }

    /* 补充二分推荐 */
    for (; j < m; j++) {
        if (bisect(minGlbId, maxGlbId, k)) {
            rsp->glb[j++] = align(k);
        }
    }

    set_head(j);
    return m_rsp;
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
        m_lost = MAX(m_lost / 2, 1);
    }

    /* 能效评估 */
    BaseAlg::notify(in, glbChId, out);
    return m_regret;
}

