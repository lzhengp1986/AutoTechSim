#ifndef MONTEALG_H
#define MONTEALG_H

#include "basealg.h"
#include "sql/kmean.h"

/* 最大搜索等级 */
#define MAX_STAGE_NUM 16

/* 搜索树大小 */
#define MAX_TREE_LEN 128
#define MAX_TREE_MSK 127

class MonteAlg : public BaseAlg
{
public:
    MonteAlg(void);
    ~MonteAlg(void);

    /* api */
    virtual void reset(void);
    virtual void restart(SqlIn& in, unsigned& failNum);
    virtual const FreqRsp& bandit(SqlIn& in, const FreqReq& req);
    virtual int notify(SqlIn& in, int glbChId, const EnvOut& out);

private:
    /*! @brief 根据band二分搜索 */
    bool bisect(int minGlbId, int maxGlbId, int& glbChId);
    /*! @brief 选择历史样本点分层聚类 */
    bool kmean(SqlIn& in, int stage);
    /*! @brief 搜索树 */
    void tree(int minGlbId, int maxGlbId);
    /*! @brief thompson统计 */
    void thomp(int glbChId, bool flag);
    /*! @brief thompson推荐 */
    int thomp(void);

private:
    int m_stage; /* 失锁标志 */
    bool m_valid[MAX_GLB_CHN];
    int m_prvGlbChId;
    KMean *m_cluster;

    /* 搜索树 */
    double m_seed[MAX_TREE_LEN];
    int m_vldNum[MAX_TREE_LEN];
    int m_invNum[MAX_TREE_LEN];
    int m_tree[MAX_TREE_LEN];
};

#endif // MONTEALG_H
