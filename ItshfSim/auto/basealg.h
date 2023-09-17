#ifndef BASEALG_H
#define BASEALG_H

#include "type.h"
#include "env/wenv.h"
#include "sql/simsql.h"

class BaseAlg
{
public:
    BaseAlg(void);
    virtual ~BaseAlg(void);

    /*! @brief 复位函数 */
    virtual void reset(void);

    /*! @brief 算法状态重置 */
    virtual void restart(const Time* ts);

    /*! @brief 推荐算法调度 */
    virtual const FreqRsp& bandit(const Time* ts, const FreqReq& req);

    /*! @brief 状态调整+性能评估 */
    virtual int notify(const Time* ts, int type, int glbChId, const EnvOut& out);

    /*! @brief 样本类型  */
    enum {SMPL_SCAN = 0, SMPL_LINK};

protected:
    static int level(int snrDelta);
    static int align(int glbChId);
    void set_head(int n);

protected:
    FreqRsp m_rsp;
    unsigned m_regret;
    static SimSql m_sql;
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
