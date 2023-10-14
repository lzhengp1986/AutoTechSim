#ifndef BISECTALG_H
#define BISECTALG_H

#include "macro.h"
#include "basealg.h"
#include "sql/kmean.h"
#include <string.h>

class BisectAlg : public BaseAlg
{
public:
    BisectAlg(void);
    ~BisectAlg(void);

    /* api */
    virtual void reset(void);
    virtual void restart(SqlIn& in, unsigned& failNum);
    virtual const FreqRsp& bandit(SqlIn& in, const FreqReq& req);
    virtual int notify(SqlIn& in, int glbChId, const EnvOut& out);

protected:
    int m_prvGlbChId;
    bool m_firstStage;
    Bisecting *m_bisect;
};

class BisectPlus : public BisectAlg
{
public:
    BisectPlus(void);
    ~BisectPlus(void);

    /* api */
    virtual void restart(SqlIn& in, unsigned& failNum);
    virtual const FreqRsp& bandit(SqlIn& in, const FreqReq& req);

private:
    /*! @brief 搜索SNR均值最优信道 */
    bool best(SqlIn& in, int& optChId);
    void swap(int i, int j);

private:
    KMean *m_cluster;
};

#endif // BISECTALG_H
