#include "macro.h"
#include "bisectalg.h"
#include <QtGlobal>

BisectAlg::BisectAlg(void)
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
    set_head(n);

    /* 附近选点 */
    rsp->glb[0] = align(m_prvGlbChId);
    int glbChId = m_prvGlbChId + qrand() % 100 - 50;
    rsp->glb[1] = align(MIN(MAX(glbChId, 0), MAX_GLB_CHN));

    /* 二分搜索 */
    for (int i = 2; i < n; i++) {
        glbChId = bisect();
        rsp->glb[i] = align(glbChId);
    }

    return m_rsp;
}

void BisectAlg::notify(bool flag, int glbChId, int snr)
{
    /* 捕获成功则切换状态 */
    if (flag == true) {
        m_firstStage = false;
    }

    /* 保存最好的信道 */
    if (snr > m_prvSnr) {
        m_prvGlbChId = glbChId;
        m_prvSnr = snr;
    }
}

int BisectAlg::bisect(bool dir)
{
    /* 限制搜索范围2M */
    int minGlbId = 0;
    int maxGlbId = MAX_GLB_CHN;
    if (m_firstStage == false) {
        minGlbId = MAX(m_prvGlbChId - 333, 0);
        maxGlbId = MIN(m_prvGlbChId + 333, MAX_GLB_CHN);
    }

    /* 二分搜索 */
    int maxLen = 0;
    int start = m_prvGlbChId;
    int stop = m_prvGlbChId;

    /* 正向 */
    int i, j, k;
    if (dir == UP) {
        for (i = start, j = start + 1; j < maxGlbId; j++) {
            if (m_valid[j] == true) {
                k = j - i - 1;
                if (k > maxLen) {
                    maxLen = k;
                    start = i;
                    stop = j;
                    i = j;
                }
            }
        }
    } else {
        for (i = start, j = start - 1; j > minGlbId; j--) {
            if (m_valid[j] == true) {
                k = i - j - 1;
                if (k > maxLen) {
                    maxLen = k;
                    start = j;
                    stop = i;
                    i = j;
                }
            }
        }
    }

    /* 二分位 */
    int glbChId;
    if (maxLen > 4) {
        int half = maxLen >> 1;
        int quart = half >> 1;
        glbChId = start + qrand() % half - quart;
    } else {
        glbChId = (start + stop) >> 1;
    }

    return glbChId;
}
