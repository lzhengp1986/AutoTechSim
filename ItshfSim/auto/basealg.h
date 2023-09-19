#ifndef BASEALG_H
#define BASEALG_H

#include "env/wenv.h"
#include "sql/simsql.h"
#include <QList>

class BaseAlg
{
public:
    BaseAlg(void);
    virtual ~BaseAlg(void);

    /*! @brief 复位函数 */
    virtual void reset(void);

    /*! @brief 算法状态重置 */
    virtual void restart(SqlIn& in);

    /*!
     * @brief 推荐算法调度
     * @param [IN] in 算法入参
     * @param [IN] req 频率请求
     * @return FreqRsp& 频率推荐表
     */
    virtual const FreqRsp& bandit(SqlIn& in, const FreqReq& req);

    /*!
     * @brief 状态调整+性能评估
     * @param [IN] in 算法入参
     * @param [IN] glbChId 信道号
     * @param [IN] out 环境评估结果
     * @return int 懊悔值
     */
    virtual int notify(SqlIn& in, int glbChId, const EnvOut& out);

    /*! @brief 样本类型  */
    enum {SMPL_SCAN = 0, SMPL_LINK};

protected:
    int middle(void);
    static int level(int snrDelta);
    static int align(int glbChId);
    void set_head(int n);

protected:
    FreqRsp m_rsp;
    unsigned m_regret; /* 懊悔值 */
    QList<int> m_kmList; /* KMean列表 */
    QList<FreqInfo> m_sqlList; /* SQL列表 */
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
