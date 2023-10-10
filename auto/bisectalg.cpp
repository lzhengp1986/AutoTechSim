#include "bisectalg.h"
#include <QtGlobal>

BisectAlg::BisectAlg(void)
{
    reset();
}

void BisectAlg::reset(void)
{
    /* 基类重置 */
    BaseAlg::reset();

    /* 初始中心 */
    m_prvGlbChId = initChId();

    /* 清状态 */
    memset(m_valid, 0, sizeof(m_valid));
    m_valid[m_prvGlbChId] = true;
    m_firstStage = true;
}

// 复位
void BisectAlg::restart(SqlIn& in)
{
    Q_UNUSED(in);

    /* 清状态 */
    m_firstStage = true;
    memset(m_valid, 0, sizeof(m_valid));
}

const FreqRsp& BisectAlg::bandit(SqlIn& in, const FreqReq& req)
{
    Q_UNUSED(in);
    FreqRsp* rsp = &m_rsp;
    int n = MIN(req.fcNum, RSP_FREQ_NUM);

    /* 300KHz附近选点 */
    int f0 = align(m_prvGlbChId);
    int f1 = chId300K(m_prvGlbChId);
    m_valid[f0] = true;
    m_valid[f1] = true;
    rsp->glb[0] = f0;
    rsp->glb[1] = f1;

    /* 限制搜索范围3M */
    int minGlbId = 0;
    int maxGlbId = MAX_GLB_CHN - 1;
    if (m_firstStage == false) {
        int schWin = BASIC_SCH_WIN / ONE_CHN_BW;
        minGlbId = MAX(m_prvGlbChId - schWin / 2, 0);
        maxGlbId = MIN(minGlbId + schWin, MAX_GLB_CHN - 1);
    }

    /* 二分搜索 */
    bool flag;
    int i, j, glbChId;
    for (i = j = 2; i < n; i++) {
        flag = bisect(minGlbId, maxGlbId, glbChId);
        if (flag == false) {
            break;
        }
        rsp->glb[j++] = align(glbChId);
    }

    set_head(j);
    return m_rsp;
}

int BisectAlg::notify(SqlIn& in, int glbChId, const EnvOut& out)
{
    if (glbChId >= MAX_GLB_CHN) {
        return m_regret;
    }

    /* 能效评估 */
    BaseAlg::notify(in, glbChId, out);

    /* 捕获成功切状态 */
    if (out.isValid == true) {
        memset(m_valid, 0, sizeof(m_valid));
        m_prvGlbChId = glbChId;
        m_firstStage = false;
    }
    return m_regret;
}

bool BisectAlg::bisect(int min, int max, int& glbChId)
{
    /* 二分搜索 */
    int maxLen = 0;
    int start = m_prvGlbChId;
    int stop = m_prvGlbChId;
    m_valid[min] = true;
    m_valid[max] = true;

    /* 正向 */
    int i, j, k;
    for (i = start, j = start + 1; j <= max; j++) {
        if (m_valid[j] == true) {
            k = j - i - 1;
            if (k > maxLen) {
                maxLen = k;
                start = i;
                stop = j;
            }
            i = j;
        }
    }

    /* 反向 */
    for (i = m_prvGlbChId, j = m_prvGlbChId - 1; j >= min; j--) {
        if (m_valid[j] == true) {
            k = i - j - 1;
            if (k > maxLen) {
                maxLen = k;
                start = j;
                stop = i;
            }
            i = j;
        }
    }

    /* 无可用频率 */
    if (maxLen <= 1) {
        return false;
    }

    /* 二分位 */
    if (maxLen > 4) {
        int half = maxLen >> 1;
        int quart = half >> 1;
        int r = rab1(0, maxLen, &m_seedi);
        glbChId = start + r % half + quart;
    } else {
        glbChId = (start + stop) >> 1;
    }

    m_valid[glbChId] = true;
    return true;
}

BisectPlus::BisectPlus(void)
{
    m_cluster = new KMean;
}

BisectPlus::~BisectPlus(void)
{
    delete m_cluster;
    m_cluster = nullptr;
}

// 重新找中心点
void BisectPlus::restart(SqlIn& in)
{
    /* 找最好的中心 */
    int optChId;
    bool flag = best(in, optChId);
    if (flag == true) {
        m_prvGlbChId = optChId;
    }

    /* 清状态 */
    m_firstStage = true;
    memset(m_valid, 0, sizeof(m_valid));
}

const FreqRsp& BisectPlus::bandit(SqlIn& in, const FreqReq& req)
{
    /* 获取历史最优 */
    int optChId;
    bool flag = best(in, optChId);
    if (flag == true) {
        m_prvGlbChId = optChId;
    }

    /* 先调用基础二分算法 */
    m_rsp = BisectAlg::bandit(in, req);

    /* 50%将二分频点提前 */
    int rnd = rab1(0, 99, &m_seedi);
    if (rnd < 50) {
        swap(0, 2);
        swap(1, 3);
    }

    return m_rsp;
}

// 找最好的中心
bool BisectPlus::best(SqlIn& in, int& optChId)
{
    /* 读取历史记录 */
    const Time* ts = in.stamp;
    in.mysql->select(SMPL_LINK, ts, in.myRule, m_sqlList);
    if (m_sqlList.isEmpty()) {
        return false;
    }

    /* 样本清零 */
    m_cluster->clear();
    int n = m_sqlList.size();

    if (m_firstStage == false) {
        int minMin = ts->min;
        int minHr = ts->hour;
        int maxMin, maxHr;

        /* case1:30min */
        if (minMin < 30) {
            /* 当前小时 */
            maxMin = minMin + 30;
            for (int i = 0; i < n; i++) {
                FreqInfo& info = m_sqlList[i];
                if (info.isNew == false) {
                    continue;
                }
                if ((info.hour == minHr)
                    && (info.min >= minMin)
                    && (info.min <= maxMin)) {
                    m_cluster->push(info);
                    info.isNew = false;
                }
            }
        } else {
            /* 当前小时+后1小时 */
            maxMin = (minMin + 30) % 60;
            maxHr = (minHr + 1) % MAX_HOUR_NUM;
            for (int i = 0; i < n; i++) {
                FreqInfo& info = m_sqlList[i];
                if (info.isNew == false) {
                    continue;
                }
                if (((info.hour == minHr) && (info.min >= minMin))
                    || ((info.hour == maxHr) && (info.min <= maxMin))) {
                    m_cluster->push(info);
                    info.isNew = false;
                }
            }
        }

        /* case1样本聚类 */
        int n30m = m_cluster->sche(m_kmList);
        if (n30m > 0) {
            optChId = m_kmList.at(0);
            return true;
        }
    }

    /* case2: 所有样本 */
    for (int i = 0; i < n; i++) {
        const FreqInfo& info = m_sqlList[i];
        if (info.isNew == false) {
            continue;
        }
        m_cluster->push(info);
    }

    /* case2样本聚类 */
    int n1h = m_cluster->sche(m_kmList);
    if (n1h > 0) {
        optChId = m_kmList.at(0);
        return true;
    }

    return false;
}

// 数据交换
inline void BisectPlus::swap(int i, int j)
{
    int t = m_rsp.glb[i];
    m_rsp.glb[i] = m_rsp.glb[j];
    m_rsp.glb[j] = t;
}

