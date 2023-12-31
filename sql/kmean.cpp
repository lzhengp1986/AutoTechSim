#include "kmean.h"
#include "sql/util.h"

KMean::KMean(int bw)
    : MAX_KM_BW(bw)
    , m_gen(new RandGen(18181))
{
    clear();
}

KMean::~KMean(void)
{
    delete m_gen;
    m_gen = nullptr;
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
    int m = opti(list);
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
int KMean::opti(QList<int>& list)
{
    int i, j, k, l;
    for(i = 0; i < m_grpNum; i++) {
        j = middle(i);
        k = m_vldIdx[j];
        l = k * CHN_SCAN_STEP;
        list.append(l);
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

    for (i = 0; i < m_grpNum; i++) {
        ind = m_grpInd + i;
        inf = m_grpInf + i;

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
            inf->avgSnr = INV_SNR;
            inf->sumSnr = 0;
            inf->beta = 0;
            continue;
        }

        /* 信息计算 */
        k = smpCnt - vldNum;
        BetaDist beta(vldNum + 1, k + 1);
        inf->beta = beta(*m_gen);
        inf->avgSnr = snrSum / smpCnt;
        inf->sumSnr = snrSum;
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
    for (i = 0; i < m_grpNum; i++) {
        for (j = i + 1, k = i; j < m_grpNum; j++) {
            l = compare(j, k);
            if (l > 0) {
                k = j;
            }
        }
        if (k != i) {
            l = m_grpIdx[k];
            m_grpIdx[k] = m_grpIdx[i];
            m_grpIdx[i] = l;
        }

        /* SNR太小时不使用 */
        j = m_grpIdx[i];
        if (m_grpInf[j].avgSnr < MIN_SNR) {
            break;
        }
    }

    m_grpNum = i;
    return i;
}

// 第i组和第j组排序比较
int KMean::compare(int i, int j)
{
    int gi = m_grpIdx[i];
    int gj = m_grpIdx[j];
    int si = m_grpInf[gi].avgSnr;
    int sj = m_grpInf[gj].avgSnr;
    if (si > sj) {
        return +1;
    } else if (si < sj) {
        return -1;
    } else {
        return 0;
    }
}

// 第i组和第j组排序比较
int KBeta::compare(int i, int j)
{
    int gi = m_grpIdx[i];
    int gj = m_grpIdx[j];
    int si = m_grpInf[gi].avgSnr;
    int sj = m_grpInf[gj].avgSnr;
    float bi = m_grpInf[gi].beta;
    float bj = m_grpInf[gj].beta;

    if ((si > MIN_SNR) && (sj > MIN_SNR)) {
        int di = (si - MIN_SNR) * bi;
        int dj = (sj - MIN_SNR) * bj;
        if (di > dj) {
            return +1;
        } else if (di < dj) {
            return -1;
        } else {
            return 0;
        }
    } else if (si > sj) {
        return +1;
    } else if (si < sj) {
        return -1;
    } else {
        return 0;
    }
}

