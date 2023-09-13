#ifndef BISECTALG_H
#define BISECTALG_H

#include "basealg.h"
#include <string.h>

class BisectAlg : public BaseAlg
{
public:
    BisectAlg(void);

    /* api */
    virtual void restart(void);
    virtual const FreqRsp& bandit(const FreqReq& req);
    virtual void notify(bool flag, int glbChId, int snr);

private:
    enum {DOWN, UP};
    int bisect(bool dir = UP);

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
}

#endif // BISECTALG_H
