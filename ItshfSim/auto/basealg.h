#ifndef BASEALG_H
#define BASEALG_H

#include "type.h"

class BaseAlg
{
public:
    virtual const FreqRsp& sche(const FreqReq& req);

private:
    FreqRsp m_rsp;
};

#endif // BASEALG_H
