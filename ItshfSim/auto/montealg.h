#ifndef MONTEALG_H
#define MONTEALG_H

#include "basealg.h"
#include "sql/kmean.h"

class MonteAlg : public BaseAlg
{
public:
    MonteAlg(void);
    ~MonteAlg(void);

    /* api */
    virtual void reset(void);
    virtual void restart(SqlIn& in);
    virtual const FreqRsp& bandit(SqlIn& in, const FreqReq& req);
    virtual int notify(SqlIn& in, int glbChId, const EnvOut& out);

private:
    /*! @brief 根据band二分搜索 */
    bool bisect(int minGlbId, int maxGlbId, int& glbChId);

private:
    int m_lost; /* 失锁标志 */
    bool m_valid[MAX_GLB_CHN];
    KMean *m_kmean;
};

#endif // MONTEALG_H
