#include "bisectalg.h"
#include <QtGlobal>

BisectAlg::BisectAlg(void)
{
    reset();
}

void BisectAlg::reset(void)
{
    /* 基类重置 */
    BaseAlg::reset();

    /* 初始中心 */
    int half = MAX_GLB_CHN / 2;
    int win = BASIC_SEARCH_WIN / ONE_CHN_BW;
    int rand = qrand() % win - (win >> 1);
    m_prvGlbChId = align(half + rand);
    m_firstStage = true;

    /* 统计清零 */
    memset(m_snrNum, 0, sizeof(m_snrNum));
    memset(m_snrSum, 0, sizeof(m_snrSum));
    memset(m_vldNum, 0, sizeof(m_vldNum));
    memset(m_invNum, 0, sizeof(m_invNum));
}

// 重新找中心点
void BisectAlg::restart(SqlIn& in)
{
    /* 找最好的中心 */
    m_prvGlbChId = best(in);

    /* 清状态 */
    memset(m_valid, 0, sizeof(m_valid));
    m_valid[m_prvGlbChId] = true;
}

const FreqRsp& BisectAlg::bandit(SqlIn& in, const FreqReq& req)
{
    Q_UNUSED(in);

    FreqRsp* rsp = &m_rsp;
    int n = req.fcNum;

    /* 300KHz附近选点 */
    int rnd = qrand() % 100;
    int glbChId = m_prvGlbChId + rnd - 50;
    rsp->glb[0] = align(MIN(MAX(glbChId, 0), MAX_GLB_CHN - 1));
    rsp->glb[1] = align(m_prvGlbChId);

    /* 搜索带宽3M/6M/9M */
    int schband;
    if (rnd < 40) { /* 40% */
        schband = BASIC_SEARCH_WIN * 1;
    } else if (rnd < 60) { /* 20% */
        schband = BASIC_SEARCH_WIN * 2;
    } else if (rnd < 80) { /* 20% */
        schband = BASIC_SEARCH_WIN * 3;
    } else { /* 20% */
        schband = BASIC_SEARCH_WIN * 4;
    }
    int schWin = schband / ONE_CHN_BW;

    /* 二分搜索 */
    int i, j;
    bool flag;
    for (i = j = 2; i < n; i++) {
        flag = bisect(schWin, glbChId);
        if (flag == false) {
            break;
        }
        rsp->glb[j++] = align(glbChId);
    }

    /* 将二分频点提前 */
    rnd = qrand() % 100;
    if (rnd < 25) {
        int tmp1 = rsp->glb[3];
        rsp->glb[3] = rsp->glb[1];
        rsp->glb[1] = tmp1;
        int tmp2 = rsp->glb[2];
        rsp->glb[2] = rsp->glb[0];
        rsp->glb[0] = tmp2;
    }

    set_head(j);
    return m_rsp;
}

int BisectAlg::notify(SqlIn& in, int glbChId, const EnvOut& out)
{
    if (glbChId >= MAX_GLB_CHN) {
        return m_regret;
    }

    /* 能效评估 */
    BaseAlg::notify(in, glbChId, out);

    /* 统计捕获信息 */
    m_prvGlbChId = best(in);

    /* 切换状态 */
    m_firstStage = false;
    return m_regret;
}

bool BisectAlg::bisect(int schband, int& glbChId)
{
    /* 限制搜索范围2M */
    int minGlbId = 0;
    int maxGlbId = MAX_GLB_CHN - 1;
    if (m_firstStage == false) {
        minGlbId = MAX(m_prvGlbChId - schband / 2, 0);
        maxGlbId = MIN(minGlbId + schband, MAX_GLB_CHN - 1);
    }

    /* 二分搜索 */
    int maxLen = 0;
    int start = m_prvGlbChId;
    int stop = m_prvGlbChId;
    m_valid[minGlbId] = true;
    m_valid[maxGlbId] = true;

    /* 正向 */
    int i, j, k;
    for (i = start, j = start + 1; j <= maxGlbId; j++) {
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

    /* 反向 */
    for (i = m_prvGlbChId, j = m_prvGlbChId - 1; j >= minGlbId; j--) {
        if (m_valid[j] == true) {
            k = i - j - 1;
            if (k > maxLen) {
                maxLen = k;
                start = j;
                stop = i;
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

// 找最好的中心
int BisectAlg::best(SqlIn& in)
{
    /*
    QList<FreqInfo> list;
    in.handle->select(SMPL_LINK, in.stamp, in.sqlMin, list);
    */

    /* 最大均值 */
    int maxIdx = 0;
    float maxAvg = avgSnr(0);
    for (int i = 1; i < MAX_GLB_CHN; i++) {
        float snr = avgSnr(i);
        if (snr > maxAvg) {
            maxAvg = snr;
            maxIdx = i;
        }
    }

    return maxIdx;
}

// 平均snr
float BisectAlg::avgSnr(int i)
{
    float avg = 0;
    if (m_snrNum[i] <= 0) {
        return avg;
    }

    avg = (float)m_snrSum[i] / m_snrNum[i];
    return avg;
}

