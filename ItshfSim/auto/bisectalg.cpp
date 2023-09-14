#include "bisectalg.h"
#include <QtGlobal>

BisectAlg::BisectAlg(void)
{
    reset();
}

void BisectAlg::reset(void)
{
    /* 中心 */
    int half = MAX_GLB_CHN / 2;
    int rand = qrand() % 1000 - 500;
    m_prvGlbChId = align(half + rand);
    m_firstStage = true;
    m_prvSnr = MIN_SNR;
}

const FreqRsp& BisectAlg::bandit(const FreqReq& req)
{
    FreqRsp* rsp = &m_rsp;
    int n = req.num;

    /* 300KHz附近选点 */
    int rnd = qrand() % 100;
    int glbChId = m_prvGlbChId + rnd - 50;
    rsp->glb[0] = align(MIN(MAX(glbChId, 0), MAX_GLB_CHN));
    rsp->glb[1] = align(m_prvGlbChId);

    /* 搜索带宽3M/6M/9M */
    int schband;
    if (rnd < 40) { /* 40% */
        schband = 3000;
    } else if (rnd < 60) { /* 20% */
        schband = 6000;
    } else if (rnd < 80) { /* 20% */
        schband = 9000;
    } else { /* 20% */
        schband = 12000;
    }

    /* 二分搜索 */
    int i, j;
    bool flag;
    for (i = j = 2; i < n; i++) {
        flag = bisect(schband, glbChId);
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

void BisectAlg::notify(bool flag, int glbChId, int snr)
{
    if (flag == false) {
        return;
    }

    /* 捕获成功则切换状态 */
    m_firstStage = false;

    /* 保存最好的信道 */
    if (snr > m_prvSnr) {
        m_prvGlbChId = glbChId;
        m_prvSnr = snr;
    }
}

bool BisectAlg::bisect(int schband, int& glbChId)
{
    /* 限制搜索范围2M */
    int minGlbId = 0;
    int maxGlbId = MAX_GLB_CHN;
    if (m_firstStage == false) {
        minGlbId = MAX(m_prvGlbChId - schband / 2, 0);
        maxGlbId = MIN(minGlbId + schband, MAX_GLB_CHN);
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
