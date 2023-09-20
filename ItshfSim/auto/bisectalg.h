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
    virtual void reset(void);
    virtual void restart(SqlIn& in);
    virtual const FreqRsp& bandit(SqlIn& in, const FreqReq& req);
    virtual int notify(SqlIn& in, int glbChId, const EnvOut& out);

private:
    /*! @brief 根据band二分搜索 */
    bool bisect(int min, int max, int& glbChId);
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

#endif // BISECTALG_H
