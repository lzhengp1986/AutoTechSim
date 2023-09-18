#ifndef KMEAN_H
#define KMEAN_H

#include "type.h"
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
    int beta; /* Beta(a,b) */
} KGInf;

class KMean
{
public:
    /*!
     * @brief 默认构造函数
     * @param [IN] bw 最大分裂带宽KHz
     */
    KMean(int bw = 300);
    void clear(void);

    /*! @brief 输入样本 */
    void push(const FreqInfo& info);

    /*! * @brief 300KHz聚类 */
    int kmean(QList<int>& list);

    /*! * @brief 频率推荐 */
    int recommend(QList<int>& list);

private:
    int group(int bw);
    int grpLen(int gid);
    int grpMid(int gid);
    int grpMid(int from, int to);
    int grpState(void);
    int grpSort(void);

    int weight(int from, int to);
    int weight(int from, int mid, int to);

private:
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
};

#endif // KMEAN_H
