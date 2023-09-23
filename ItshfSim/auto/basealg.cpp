#include "basealg.h"

// 构造
BaseAlg::BaseAlg(void)
{
    set_head(0);
    m_regret = 0;
    m_kmList.clear();
    m_sqlList.clear();
}

// 析构
BaseAlg::~BaseAlg(void)
{
}

// 复位函数
void BaseAlg::reset(void)
{
    m_regret = 0;
}

// 算法状态重置
void BaseAlg::restart(SqlIn& in)
{
    Q_UNUSED(in);
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
        j = m_randi.rab(0, MAX_GLB_CHN - 1);
        rsp->glb[i] = align(j);
    }

    return m_rsp;
}

// 15MHz附近产生随机信道
int BaseAlg::initChId(void)
{
    int half = MAX_GLB_CHN / 2;
    int win = BASIC_SCH_WIN / ONE_CHN_BW;
    int r = m_randi.rab(0, MAX_GLB_CHN);
    int rand = r % win - (win >> 1);
    return align(half + rand);
}

// 300KHz附近产生随机信道
int BaseAlg::chId300K(int chId)
{
    int rnd = m_randi.rab(0, FST_RND_RNG - 1);
    int glbChId = MAX(chId + rnd - FST_RND_RNG / 2, 0);
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
    bool mufVld = out.mufVld;
    int mufSnr = out.mufSnr;

    /* 性能差异比较 */
    int delta;
    if ((frqVld == true) && (mufVld == true)) {
        delta = mufSnr - frqSnr;
    } else if ((frqVld == true) && (mufVld == false)) {
        delta = -frqSnr;
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

