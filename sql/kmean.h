#ifndef KMEAN_H
#define KMEAN_H

#include "type.h"
#include "util.h"
#include <QList>

/* 压缩信道个数：5个一组 */
#define MAX_KM_NUM (MAX_GLB_CHN / CHN_SCAN_STEP + 1)

// 组索引
typedef struct {
    int from; /* local index */
    int to; /* local index */
} KGInd;

// 组信息
typedef struct {
    int sumSnr; /* 样本和 */
    int avgSnr; /* 均值 */
    float beta; /* Beta(a,b) */
} KGInf;

class KMean
{
public:
    /*!
     * @brief 默认构造函数
     * @param [IN] bw 最大分裂带宽KHz
     */
    KMean(int bw = 300);
    virtual ~KMean(void);

    /*! @brief 清样本 */
    void clear(void);

    /*! @brief 输入样本 */
    void push(const FreqInfo& info);

    /*!
     * @brief 300KHz聚类
     * @param [INOUT] list 推荐列表
     * @return int 推荐个数
     */
    int sche(QList<int>& list);

    /*!
     * @brief 信道推荐
     * @param [INOUT] list 推荐列表
     * @return int 推荐个数
     */
    int opti(QList<int>& list);

protected:
    /*!
     * @brief 第i组和第j组排序比较
     * @param [IN] i 第i组索引
     * @param [IN] j 第j组索引
     * @return int 比较结果
     */
    virtual int compare(int i, int j);

protected:
    int group(int bw);
    int length(int gid);
    int middle(int gid);
    int middle(int from, int to);
    int weight(int from, int to);
    int weight(int gid);
    int state(void);
    int sort(void);

protected:
    /* 原始样本 */
    const int MAX_KM_BW;
    int m_snrSum[MAX_KM_NUM];
    int m_smpCnt[MAX_KM_NUM];
    int m_vldCnt[MAX_KM_NUM];

    /* 有效样本映射 */
    int m_vldNum, m_grpNum;
    int m_vldIdx[MAX_KM_NUM];
    int m_grpIdx[MAX_KM_NUM];
    KGInd m_grpInd[MAX_KM_NUM];
    KGInf m_grpInf[MAX_KM_NUM];
    RandGen* m_rand;
};

// 以加权SNR均值排序
class KBeta : public KMean
{
protected:
    virtual int compare(int i, int j);
};

#endif // KMEAN_H
