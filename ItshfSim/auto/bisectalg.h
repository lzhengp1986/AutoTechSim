#ifndef BISECTALG_H
#define BISECTALG_H

#include "macro.h"
#include "basealg.h"
#include <string.h>

class BisectAlg : public BaseAlg
{
public:
    BisectAlg(void);

    /* api */
    void reset(void);
    void restart(void);
    virtual const FreqRsp& bandit(const FreqReq& req);
    void notify(bool flag, int glbChId, int snr);

private:
    /*! @brief 根据band二分搜索 */
    bool bisect(int schband, int& glbChId);

private:
    int m_prvSnr;
    int m_prvGlbChId;
    bool m_firstStage;
    bool m_valid[MAX_GLB_CHN];
};

// 重新启动
inline void BisectAlg::restart(void)
{
    memset(m_valid, 0, sizeof(m_valid));
    m_valid[m_prvGlbChId] = true;
    m_prvSnr = MIN_SNR;
}

#endif // BISECTALG_H
