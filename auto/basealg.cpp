#include "basealg.h"

// 构造
BaseAlg::BaseAlg(void)
{
    reset();
}

// 析构
BaseAlg::~BaseAlg(void)
{
}

// 复位函数
void BaseAlg::reset(void)
{
    set_head(0);
    m_regret = 0;
    m_seedi = 1987;
    m_kmList.clear();
    m_sqlList.clear();
    m_positive = true;
}

// 算法状态重置
void BaseAlg::restart(SqlIn& in, unsigned& failNum)
{
    Q_UNUSED(in);
    Q_UNUSED(failNum);
}

// 随机搜索
const FreqRsp& BaseAlg::bandit(SqlIn& in, const FreqReq& req)
{
    Q_UNUSED(in);
    FreqRsp* rsp = &m_rsp;
    int n = req.fcNum;
    set_head(n);

    int i, j;
    for (i = 0; i < n; i++) {
        j = rab1(0, MAX_GLB_CHN - 1, &m_seedi);
        rsp->glb[i] = align(j);
    }

    return m_rsp;
}

// 15MHz附近产生随机信道
int BaseAlg::initChId(void)
{
    int chWin = BASIC_SCH_WIN / ONE_CHN_BW;
    int r = rab1(0, MAX_GLB_CHN - 1, &m_seedi);
    return align(MAX_GLB_CHN / 2 - (chWin >> 1) + (r % chWin));
}

// 300KHz附近产生随机信道
int BaseAlg::chId300K(int chId)
{
    const int rndRng = 100;
    int rnd = rab1(0, rndRng, &m_seedi);
    int glbChId = MAX(chId + rnd - rndRng / 2, 0);
    return align(MIN(glbChId, MAX_GLB_CHN - 1));
}

// snr差值分段
int BaseAlg::level(int delta)
{
    int lev;
    if (delta < -15) {
        lev = -7;
    } else if (delta < -10) {
        lev = -5;
    } else if (delta < -5) {
        lev = -3;
    } else if (delta < 0) {
        lev = -1;
    } else {
        lev = delta;
    }

    return lev;
}

// 能效评估
int BaseAlg::notify(SqlIn& in, int glbChId, const EnvOut& out)
{
    Q_UNUSED(in);
    Q_UNUSED(glbChId);

    int frqSnr = out.snr;
    bool frqVld = out.isValid;
    bool fotVld = out.fotVld;
    int fotSnr = out.fotSnr;

    /* 性能差异比较 */
    int delta;
    if ((frqVld == true) && (fotVld == true)) {
        delta = fotSnr - frqSnr;
    } else if ((frqVld == true) && (fotVld == false)) {
        delta = -frqSnr;
    } else if ((frqVld == false) && (fotVld == true)) {
        delta = fotSnr;
    } else {
        delta = 0;
    }

    /* 懊悔值累加 */
    int reg = level(delta);
    m_regret += reg;
    return m_regret;
}

