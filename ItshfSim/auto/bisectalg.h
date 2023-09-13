#ifndef BISECTALG_H
#define BISECTALG_H

#include "basealg.h"

class BisectAlg : public BaseAlg
{
public:
    BisectAlg(void);
    virtual const FreqRsp& bandit(const FreqReq& req);
    virtual void notify(int glbChId, int snr);

private:
    int m_hist;
};

#endif // BISECTALG_H
