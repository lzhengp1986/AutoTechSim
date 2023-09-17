#ifndef BISECTALG_H
#define BISECTALG_H

#include "macro.h"
#include "basealg.h"
#include <string.h>

/* 扫频最大失败次数 */
#define MAX_SCAN_FAIL_THR 5
/* 基本探索窗KHz */
#define BASIC_SEARCH_WIN 3000

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
    bool bisect(int schband, int& glbChId);
    int best(SqlIn& in);
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
