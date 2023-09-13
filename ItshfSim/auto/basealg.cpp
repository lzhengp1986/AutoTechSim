#include "basealg.h"
#include <QtGlobal>

// 默认随机搜索
const FreqRsp& BaseAlg::bandit(const FreqReq& req)
{
    FreqRsp* rsp = &m_rsp;
    int n = req.num;
    rsp->head.type = MSG_FREQ_RSP;
    rsp->used = 0;
    rsp->total = n;

    for (int i = 0; i < n; i++) {
        rsp->glb[i] = qrand() % MAX_GLB_CHN;
    }

    return m_rsp;
}

// 信息
void BaseAlg::notify(int glbChId, int snr)
{
    Q_UNUSED(glbChId); Q_UNUSED(snr);
}
