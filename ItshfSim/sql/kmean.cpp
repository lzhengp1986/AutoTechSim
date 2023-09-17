#include "kmean.h"

KMean::KMean()
{
    clear();
}

// 样本清零
void KMean::clear(void)
{
    memset(m_snrSum, 0, sizeof(m_snrSum));
    memset(m_smpCnt, 0, sizeof(m_smpCnt));
    memset(m_vldCnt, 0, sizeof(m_vldCnt));
}

// 输入样本
void KMean::push(bool valid, int glbChId, int snr)
{
    int locChId = glbChId / CHN_SCAN_STEP;
    m_vldCnt[locChId] += (valid == true);
    m_snrSum[locChId] += snr;
    m_smpCnt[locChId]++;
}

// 300KHz聚类
int KMean::kmean(int n, int bw)
{
    /* 聚类分组 */
    int grp1 = group(bw);
    if (grp1 <= 0) {
        return 0;
    }

    /* 信息统计 */
    int grp2 = grpState();
    int m = MIN(grp2, n);

    /* 组排序 */
    int grp3 = grpSort(m);
    return grp3;
}

// 分组
int KMean::group(int bw)
{
    int i, j, k;

    /* step1.有效样本映射 */
    for (i = 0, j = 0; i < MAX_KM_NUM; i++) {
        if (m_smpCnt[i] > 0) {
            m_vldIdx[j++] = i;
        }
    }

    /* step2.样本数 */
    if (j <= 0) {
        m_vldNum = 0;
        m_grpNum = 0;
        return 0;
    }

    /* step3.第1组 */
    m_grpInd[0].from = m_vldIdx[0];
    m_grpInd[0].to = m_vldIdx[j - 1];
    m_grpNum = 1;
    m_vldNum = j;

    /* step4.计算门限 */
    int thr = bw / CHN_SCAN_STEP / ONE_CHN_BW;

    /* step5.二分迭代分组 */
    while (true) {
        /* 找最大组 */
        int mgid = 0;
        int maxLen = grpLen(mgid);
        for (i = 1; i < m_grpNum; i++) {
            j = grpLen(i);
            if (j > maxLen) {
                maxLen = j;
                mgid = i;
            }
        }
        if (maxLen <= thr) {
            break;
        }

        /* 找分裂点 */
        KGInd *grp = m_grpInd + mgid;
        int maxWt = weight(grp->from, grp->to);
        for (i = k = grp->from + 1; i <= grp->to; i++) {
            j = weight(grp->from, i) + weight(i + 1, grp->to);
            if (j < maxWt) {
                maxWt = j;
                k = i;
            }
        }

        /* 挪出组信息 */
        for (i = m_grpNum; i > mgid; i--) {
            m_grpInd[i] = m_grpInd[i - 1];
        }

        /* 将其分裂 */
        m_grpInd[mgid].to = k;
        m_grpInd[mgid + 1].from = k + 1;
        m_grpNum++;
    }

    return m_grpNum;
}

// 频率推荐
bool KMean::recommend(QList<int>& list)
{

}

// 组长度
inline int KMean::grpLen(int gid)
{
    int from = m_grpInd[gid].from;
    int to = m_grpInd[gid].to;
    return to - from + 1;
}

// 组中心
inline int KMean::grpMid(int gid)
{
    int from = m_grpInd[gid].from;
    int to = m_grpInd[gid].to;
    return grpMid(from, to);
}

// 组中心
inline int KMean::grpMid(int from, int to)
{
    return ((from + to) >> 1);
}

// 权重计算
inline int KMean::weight(int from, int to)
{
    int mid = grpMid(from, to);
    return weight(from, mid, to);
}

// 权重计算
inline int KMean::weight(int from, int mid, int to)
{
    int i, j, k, w = 0;
    int x = m_vldIdx[mid];
    for (i = from; i <= to; i++) {
        j = m_vldIdx[i];
        k = m_smpCnt[j];
        w += k * ABS(j - x);
    }
    return w;
}

// 统计
int KMean::grpState(void)
{
    int i, j, k;
    KGInd* ind;
    KGInf* inf;

    int snrSum = 0;
    int smpCnt = 0;
    int vldNum = 0;

    for (i = 0; i < m_grpNum; i++) {
        ind = m_grpInd + i;
        inf = m_grpInf + i;

        /* 样本统计 */
        for (j = ind->from; j <= ind->to; j++) {
            k = m_vldIdx[j];
            snrSum += m_snrSum[k];
            smpCnt += m_smpCnt[k];
            vldNum += m_vldCnt[k];
        }

        /* 信息计算 */
        inf->avgSnr = snrSum / smpCnt;
        inf->coefB = smpCnt - vldNum;
        inf->coefA = vldNum;
    }

    return m_grpNum;
}

// 排序
int KMean::grpSort(int n)
{
    int i, j, k, l;

    /* 初始化索引 */
    int grpNum = m_grpNum;
    for (i = 0; i < m_grpNum; i++) {
        m_grpIdx[i] = i;
    }

    /* 组排序 */
    int sj, sk;
    for (i = 0; i < n - 1; i++) {
        for (j = i + 1, k = i; j < grpNum; j++) {
            sj = m_grpInf[m_grpIdx[j]].avgSnr;
            sk = m_grpInf[m_grpIdx[k]].avgSnr;
            if (sj > sk) {
                k = j;
            }
        }
        if (k != i) {
            l = m_grpIdx[k];
            m_grpIdx[k] = m_grpIdx[i];
            m_grpIdx[i] = l;
        }
    }

    return n;
}
