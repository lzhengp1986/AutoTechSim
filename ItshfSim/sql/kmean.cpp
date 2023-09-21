#include "kmean.h"
#include "beta.h"
#include <QtGlobal>

KMean::KMean(int bw)
    : MAX_KM_BW(bw)
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
void KMean::push(const FreqInfo& info)
{
    int locChId = info.glbChId / CHN_SCAN_STEP;
    m_vldCnt[locChId] += (info.valid == true);
    m_snrSum[locChId] += info.snr;
    m_smpCnt[locChId] ++;
}

// 300KHz聚类
int KMean::sche(QList<int>& list)
{
    /* 聚类分组 */
    list.clear();
    int gn = group(MAX_KM_BW);
    if (gn <= 0) {
        return 0;
    }

    /* 排序 */
    state();
    sort();

    /* 频率推荐 */
    int m = recommend(list);
    return m;
}

// 分组
int KMean::group(int bw)
{
    int i, j, k;

    /* step1.有效样本映射 */
    for (i = 0, j = 0; i < MAX_KM_NUM; i++) {
        if ((m_smpCnt[i] > 0) && (m_vldCnt[i] > 0)) {
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
    m_grpInd[0].from = 0;
    m_grpInd[0].to = j - 1;
    m_grpNum = 1;
    m_vldNum = j;

    /* step4.二分迭代分组 */
    int thr =bw / ONE_CHN_BW / CHN_SCAN_STEP;
    while (true) {
        /* 找最大组 */
        int mgid = 0;
        int maxLen = length(mgid);
        for (i = 1; i < m_grpNum; i++) {
            j = length(i);
            if (j > maxLen) {
                maxLen = j;
                mgid = i;
            }
        }
        if (maxLen <= thr) {
            break;
        }

        /* 找分裂点 */
        int maxWt = weight(mgid);
        KGInd *grp = m_grpInd + mgid;
        for (i = k = grp->from; i <= grp->to; i++) {
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
int KMean::recommend(QList<int>& list)
{
    int i, j, k;
    for(i = 0; i < m_grpNum; i++) {
        j = middle(i);
        k = m_vldIdx[j];
        list.append(k * CHN_SCAN_STEP);
    }

    return list.size();
}

// 组长度
inline int KMean::length(int gid)
{
    int from = m_vldIdx[m_grpInd[gid].from];
    int to = m_vldIdx[m_grpInd[gid].to];
    return to - from + 1;
}

// 组中心
inline int KMean::middle(int gid)
{
    int from = m_grpInd[gid].from;
    int to = m_grpInd[gid].to;
    return middle(from, to);
}

// 组中心
inline int KMean::middle(int from, int to)
{
    /* 样本统计 */
    int i, j, k;
    for (i = from, k = 0; i <= to; i++) {
        k += m_smpCnt[m_vldIdx[i]];
    }

    /* 找中值 */
    j = (k >> 1);
    for (i = from, k = 0; i <= to; i++) {
        k += m_smpCnt[m_vldIdx[i]];
        if (k >= j) {
            break;
        }
    }

    return i;
}

// 权重计算
inline int KMean::weight(int gid)
{
    int mid = middle(gid);
    int from = m_grpInd[gid].from;
    int to = m_grpInd[gid].to;

    int i, j, k, w;
    int l = m_vldIdx[mid];
    for (i = from, w = 0; i <= to; i++) {
        j = m_vldIdx[i];
        k = m_smpCnt[j];
        w += k * ABS(j - l);
    }

    return w;
}

// 权重计算
inline int KMean::weight(int from, int to)
{
    int i, j, k, w;
    int mid = middle(from, to);
    int l = m_vldIdx[mid];

    for (i = from, w = 0; i <= to; i++) {
        j = m_vldIdx[i];
        k = m_smpCnt[j];
        w += k * ABS(j - l);
    }

    return w;
}

// 统计
int KMean::state(void)
{
    int i, j, k;
    KGInd* ind;
    KGInf* inf;

    int snrSum = 0;
    int smpCnt = 0;
    int vldNum = 0;

    double py;
    int rnd = qrand();
    double px = (double)rnd / RAND_MAX;
    for (i = 0; i < m_grpNum; i++) {
        ind = m_grpInd + i;
        inf = m_grpInf + i;
        inf->avgSnr = INV_SNR;
        inf->sumSnr = 0;
        inf->beta = 0;

        /* 样本统计 */
        snrSum = smpCnt = vldNum = 0;
        for (j = ind->from; j <= ind->to; j++) {
            k = m_vldIdx[j];
            snrSum += m_snrSum[k];
            smpCnt += m_smpCnt[k];
            vldNum += m_vldCnt[k];
        }

        /* 异常保护 */
        if (smpCnt <= 0) {
            continue;
        }

        /* 信息计算 */
        k = smpCnt - vldNum;
        py = beta(vldNum, k, px);
        inf->sumSnr = snrSum;
        inf->avgSnr = snrSum / smpCnt;
        inf->beta = (float)py;
    }

    return m_grpNum;
}

// 排序
int KMean::sort(void)
{
    int i, j, k, l;

    /* 初始化索引 */
    for (i = 0; i < m_grpNum; i++) {
        m_grpIdx[i] = i;
    }

    /* 组排序 */
    int sj, sk;
    for (i = 0; i < m_grpNum; i++) {
        for (j = i + 1, k = i; j < m_grpNum; j++) {
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

        /* SNR太小时不使用 */
        sk = m_grpInf[m_grpIdx[i]].avgSnr;
        if (sk < MIN_SNR) {
            break;
        }
    }

    m_grpNum = i;
    return i;
}
