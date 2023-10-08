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

protected:
    /*! @brief 根据band二分搜索 */
    bool bisect(int min, int max, int& glbChId);

protected:
    int m_prvGlbChId;
    bool m_firstStage;
    bool m_valid[MAX_GLB_CHN];
};

class BisectPlus : public BisectAlg
{
public:
    virtual void restart(SqlIn& in);
    virtual const FreqRsp& bandit(SqlIn& in, const FreqReq& req);
    virtual int notify(SqlIn& in, int glbChId, const EnvOut& out);

private:
    /*! @brief 搜索SNR均值最优信道 */
    bool best(SqlIn& in, int& optChId);
    void swap(int i, int j);
    float avgSnr(int i);

private:
    /* SNR统计 */
    int m_snrSum[MAX_GLB_CHN];
    int m_snrNum[MAX_GLB_CHN];
};

#endif // BISECTALG_H
