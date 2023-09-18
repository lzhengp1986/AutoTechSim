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
    bool bisect(int schband, int& glbChId);
    bool best(SqlIn& in, int& optChId);

private:
    int m_prvGlbChId;
    bool m_firstStage;
    bool m_valid[MAX_GLB_CHN];
    KMean *m_kmean;
};

#endif // MONTEALG_H
