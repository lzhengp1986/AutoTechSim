#include "basealg.h"

// 构造
BaseAlg::BaseAlg(void)
{
    int seed[] = {1987, 1997, 2017, 2087, 2137};
    for (int i = 0; i < NUM; i++) {
        m_gen[i] = new RandGen(seed[i]);
    }
    reset();
}

// 析构
BaseAlg::~BaseAlg(void)
{
    for (int i = 0; i < NUM; i++) {
        delete m_gen[i];
        m_gen[i] = nullptr;
    }
}

// 复位函数
void BaseAlg::reset(void)
{
    set_head(0);
    m_regret = 0;
    m_kmList.clear();
    m_sqlList.clear();
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
    UnifIntDist dist(0, MAX_GLB_CHN - 1);
    for (i = 0; i < n; i++) {
        j = dist(*m_gen[ALG]);
        rsp->glb[i] = align(j);
    }

    return m_rsp;
}

// 15MHz附近产生随机信道
int BaseAlg::initChId(void)
{
    /* 随机数 */
    int chWin = BASIC_SCH_WIN / ONE_CHN_BW;
    UnifIntDist dist(0, chWin - 1);
    int rnd = dist(*m_gen[INIT]);

    /* 频段中心随机偏移 */
    return align(MAX_GLB_CHN / 2 - (chWin >> 1) + rnd);
}

// 300KHz附近产生随机信道
int BaseAlg::chId300K(int chId)
{
    /* 随机数 */
    const int rndRng = 100;
    UnifIntDist dist(0, rndRng);
    int rnd = dist(*m_gen[K300]);

    /* 随机偏移 */
    int glbChId = MAX(chId + rnd - (rndRng >> 1), 0);
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

Bisecting::Bisecting(void)
{
    m_gen[DIR] = new RandGen(1987);
    m_gen[ALG] = new RandGen(1987);
    clear();
}

Bisecting::~Bisecting(void)
{
    delete m_gen[DIR];
    delete m_gen[ALG];
    m_gen[DIR] = nullptr;
    m_gen[ALG] = nullptr;
}

// 二分随机推荐算法
bool Bisecting::sche(int min, int max, int& glbChId)
{
    int i, j, k;
    int start, stop;
    int maxLen = -1;

    m_valid[min] = true;
    m_valid[max] = true;

    /* 二分搜索 */
    BernDist dist;
    int dir = dist(*m_gen[DIR]);
    if (dir == 0) {
        start = stop = min;
        for (i = min, j = min + 1; j <= max; j++) {
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
    } else {
        start = stop = max;
        for (i = max, j = max - 1; j >= min; j--) {
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
    }

    /* 无可用频率 */
    if (maxLen <= 1) {
        return false;
    }

    /* 二分位 */
    int median;
    if (maxLen > 4) {
        int half = maxLen >> 1;
        int quart = half >> 1;
        UnifIntDist dist(0, half);
        int rnd = dist(*m_gen[ALG]);
        median = start + rnd + quart;
    } else {
        median = (start + stop) >> 1;
    }

    glbChId = BaseAlg::align(median);
    m_valid[glbChId] = true;
    return true;
}

