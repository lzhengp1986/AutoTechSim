#ifndef BASEALG_H
#define BASEALG_H

#include "type.h"

class BaseAlg
{
public:
    virtual ~BaseAlg(void) {}

    /* 算法调度 */
    virtual const FreqRsp& bandit(const FreqReq& req);

protected:
    static int align(int glbChId);
    void set_head(int n);

protected:
    FreqRsp m_rsp;
};

// 填写消息头
inline void BaseAlg::set_head(int n)
{
    m_rsp.head.type = MSG_FREQ_RSP;
    m_rsp.total = n;
    m_rsp.used = 0;
}

// 信道对齐
inline int BaseAlg::align(int glbChId)
{
    return (glbChId / CHN_SCAN_STEP) * CHN_SCAN_STEP;
}

#endif // BASEALG_H
