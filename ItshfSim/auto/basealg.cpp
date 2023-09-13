#include "basealg.h"
#include <QtGlobal>

// 默认随机搜索
const FreqRsp& BaseAlg::bandit(const FreqReq& req)
{
    FreqRsp* rsp = &m_rsp;
    int n = req.num;
    set_head(n);

    int glbChId;
    for (int i = 0; i < n; i++) {
        glbChId = qrand() % MAX_GLB_CHN;
        rsp->glb[i] = align(glbChId);
    }

    return m_rsp;
}

// 信息
void BaseAlg::notify(bool flag, int glbChId, int snr)
{
    Q_UNUSED(flag); Q_UNUSED(glbChId); Q_UNUSED(snr);
}
