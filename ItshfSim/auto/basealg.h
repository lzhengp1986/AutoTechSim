#ifndef BASEALG_H
#define BASEALG_H

#include "type.h"

class BaseAlg
{
public:
    /* 算法调度 */
    virtual const FreqRsp& bandit(const FreqReq& req);
    virtual void notify(int glbChId, int snr);

private:
    FreqRsp m_rsp;
};

#endif // BASEALG_H
