#include "basealg.h"
#include <QtGlobal>

// 默认随机搜索
const FreqRsp& BaseAlg::sche(const FreqReq& req)
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
