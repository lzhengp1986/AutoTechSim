#ifndef MONTEALG_H
#define MONTEALG_H

#include "basealg.h"

class MonteAlg : public BaseAlg
{
public:
    MonteAlg(void);

    /* api */
    virtual void reset(void);
    virtual void restart(SqlIn& in);
    virtual const FreqRsp& bandit(SqlIn& in, const FreqReq& req);
    virtual int notify(SqlIn& in, int glbChId, const EnvOut& out);

private:
    /*! @brief 根据band二分搜索 */
    bool bisect(int schband, int& glbChId);
    bool best(SqlIn& in, int& optChId);
    float avgSnr(int i);

private:
    int m_prvGlbChId;
    bool m_firstStage;
    bool m_valid[MAX_GLB_CHN];

    /* SNR统计 */
    int m_snrSum[MAX_GLB_CHN];
    int m_snrNum[MAX_GLB_CHN];

    /* thompson可用率统计 */
    int m_vldNum[MAX_GLB_CHN];
    int m_invNum[MAX_GLB_CHN];
};

#endif // MONTEALG_H
