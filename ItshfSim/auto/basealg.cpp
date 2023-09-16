#include "basealg.h"
#include <QtGlobal>
#include <QTime>

// 实例化
SimSql BaseAlg::m_sql;

// 构造
BaseAlg::BaseAlg(void)
{
    set_head(0);
    m_regret = 0;
}

// 析构
BaseAlg::~BaseAlg(void)
{
}

// 复位函数
void BaseAlg::reset(void)
{
    m_regret = 0;
    m_sql.drop(SMPL_SCAN);
    m_sql.drop(SMPL_LINK);
}

// 算法状态重置
void BaseAlg::restart(void)
{
    int msec = QTime::currentTime().msecsSinceStartOfDay();
    qsrand((uint)msec);
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
int BaseAlg::level(int delta)
{
    int lev;
    if (delta < -10) {
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
int BaseAlg::notify(int type, const Time* ts, int glbChId, const EnvOut& out)
{
    Q_UNUSED(ts);
    Q_UNUSED(glbChId);

    int frqSnr = out.snr;
    bool frqVld = out.isValid;
    bool mufVld = out.mufVld;
    int mufSnr = out.mufSnr;

    /* 记录到sql */
    int ret = m_sql.insert(type, ts, frqVld, glbChId, frqSnr, out.n0);
    if (ret != SQLITE_OK) {
        /* nothing-to-do */
    }

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

