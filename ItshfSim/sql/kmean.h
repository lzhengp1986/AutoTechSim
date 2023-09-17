#ifndef KMEAN_H
#define KMEAN_H

#include "macro.h"
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
    int avgSnr; /* 均值 */
    int coefA; /* Beta(a,b) */
    int coefB; /* Beta(a,b) */
} KGInf;

class KMean
{
public:
    KMean(void);
    void clear(void);

    /*! @brief 输入样本 */
    void push(bool valid, int glbChId, int snr);

    /*!
     * @brief 300KHz聚类
     * @param [IN] n 最多n类
     * @return int 实际类数
     */
    int kmean(int n, int bw = 300);

    /*!
     * @brief 频率推荐
     * @param [IN] list 频率列表
     * @return bool 成功与否
     */
    bool recommend(QList<int>& list);

private:
    int group(int bw);
    int grpLen(int gid);
    int grpMid(int gid);
    int grpMid(int from, int to);
    int grpState(void);
    int grpSort(int n);

    int weight(int from, int to);
    int weight(int from, int mid, int to);

private:
    /* 原始样本 */
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
