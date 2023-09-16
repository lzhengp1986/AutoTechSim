#include "basealg.h"
#include <QtGlobal>

// 构造
BaseAlg::BaseAlg(void)
{
    set_head(0);
    m_regret = 0;
}

// 随机搜索
const FreqRsp& BaseAlg::bandit(const FreqReq& req)
{
    FreqRsp* rsp = &m_rsp;
    int n = req.fcNum;
    set_head(n);

    int glbChId;
    for (int i = 0; i < n; i++) {
        glbChId = qrand() % MAX_GLB_CHN;
        rsp->glb[i] = align(glbChId);
    }

    return m_rsp;
}

// snr差值分段
int BaseAlg::level(int snrDelta)
{
    int lev;
    if (snrDelta > 3) {
        lev = -1;
    } else {
        lev = -snrDelta;
    }

    return lev;
}

// 能效评估
int BaseAlg::notify(const Time* ts, int glbChId, const EnvOut& out)
{
    Q_UNUSED(ts);
    Q_UNUSED(glbChId);

    int frqSnr = out.snr;
    bool frqVld = out.isValid;
    bool mufVld = out.mufVld;
    int mufSnr = out.mufSnr;

    /* 性能差异比较 */
    int delta;
    if ((frqVld == true) && (mufVld == true)) {
        delta = frqSnr - mufSnr;
    } else if ((frqVld == true) && (mufVld == false)) {
        delta = frqSnr;
    } else if ((frqVld == false) && (mufVld == true)) {
        delta = mufSnr;
    } else {
        delta = 0;
    }

    /* 懊悔值累加 */
    int reg = level(delta);
    m_regret += reg;
    return m_regret;
}

