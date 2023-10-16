#ifndef BASEALG_H
#define BASEALG_H

#include "env/wenv.h"
#include "env/randmng.h"
#include "sql/simsql.h"
#include "sql/util.h"
#include <QList>

/* 基础搜索带宽KHz */
#define BASIC_SCH_WIN 2000

class BaseAlg
{
public:
    BaseAlg(void);
    virtual ~BaseAlg(void);

    /*! @brief 复位函数 */
    virtual void reset(void);

    /*! @brief 算法状态重置 */
    virtual void restart(SqlIn& in, unsigned& failNum);

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

    /*! @brief 信道号按15KHz对齐 */
    static int align(int glbChId);

protected:
    int initChId(void);
    int chId300K(int chId);
    static int level(int snrDelta);
    void set_head(int n);

protected:
    FreqRsp m_rsp;
    unsigned m_regret; /* 懊悔值 */
    QList<int> m_kmList; /* KMean列表 */
    QList<FreqInfo> m_sqlList; /* SQL列表 */

    enum {INIT = 0, K300, ALG, RSV, NUM};
    RandGen *m_gen[NUM]; /* 随机数生成器 */
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

// 基本二分算法
class Bisecting
{
public:
    Bisecting(void);
    ~Bisecting(void);

    /*! @brief 清标志 */
    void clear(void);
    /*! @brief 置标志 */
    void setValid(int glbChId);

    /*!
     * @brief 二分随机推荐算法
     * @param [IN] min 最小全局信道号
     * @param [IN] max 最大全局信道号
     * @param [INOUT] glbChId 推荐信道
     * @return bool 推荐是否有效
     */
    bool sche(int min, int max, int& glbChId);

private:
    bool m_valid[MAX_GLB_CHN];
    enum {DIR = 0, ALG, NUM};
    RandGen *m_gen[NUM];
};

inline void Bisecting::clear(void)
{
    memset(m_valid, 0, sizeof(m_valid));
}

inline void Bisecting::setValid(int glbChId)
{
    m_valid[glbChId] = true;
}

#endif // BASEALG_H
