#include "bisectalg.h"
#include <QtGlobal>

BisectAlg::BisectAlg(void)
    : m_bisect(new Bisecting)
{
    reset();
}

BisectAlg::~BisectAlg(void)
{
    delete m_bisect;
    m_bisect = nullptr;
}

void BisectAlg::reset(void)
{
    /* 基类重置 */
    BaseAlg::reset();

    /* 初始中心 */
    m_prvGlbChId = initChId();

    /* 清状态 */
    m_firstStage = true;
    m_bisect->clear();
}

// 复位
void BisectAlg::restart(SqlIn& in, unsigned& failNum)
{
    Q_UNUSED(in);
    /* 短二每次探210个频率 */
    const int restartThr = 20;
    if (failNum >= restartThr) {
        m_firstStage = true;
        m_bisect->clear();
        failNum = 0;
    }
}

const FreqRsp& BisectAlg::bandit(SqlIn& in, const FreqReq& req)
{
    Q_UNUSED(in);
    FreqRsp* rsp = &m_rsp;
    int n = MIN(req.fcNum, RSP_FREQ_NUM);

    /* 300KHz附近选点 */
    int f0 = align(m_prvGlbChId);
    int f1 = chId300K(m_prvGlbChId);
    m_bisect->setValid(f0);
    m_bisect->setValid(f1);
    rsp->glb[0] = f0;
    rsp->glb[1] = f1;

    /* 限制搜索范围 */
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
        flag = m_bisect->sche(minGlbId, maxGlbId, glbChId);
        if (flag == false) {
            break;
        }
        rsp->glb[j++] = glbChId;
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
        m_bisect->setDir(glbChId > m_prvGlbChId);
        m_prvGlbChId = glbChId;
        m_firstStage = false;
        m_bisect->clear();
    }
    return m_regret;
}

BisectPlus::BisectPlus(void)
    : m_cluster(new KMean)
{
}

BisectPlus::~BisectPlus(void)
{
    delete m_cluster;
    m_cluster = nullptr;
}

// 重新找中心点
void BisectPlus::restart(SqlIn& in, unsigned& failNum)
{
    const int restartThr = 5;
    if (failNum >= restartThr) {
        /* 找最好的中心 */
        int optChId;
        bool flag = best(in, optChId);
        if (flag == true) {
            m_prvGlbChId = optChId;
        }

        /* 清状态 */
        m_firstStage = true;
        m_bisect->clear();
        failNum = 0;
    }
}

const FreqRsp& BisectPlus::bandit(SqlIn& in, const FreqReq& req)
{
    /* 获取历史最优 */
    int optChId;
    bool flag = best(in, optChId);
    if (flag == true) {
        m_prvGlbChId = optChId;
    }

    /* 基础二分算法 */
    m_rsp = BisectAlg::bandit(in, req);
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
        int i, minMin, minHr, maxMin, maxHr;

        /* case1:30min */
        if (ts->min >= 15) {
            minHr = ts->hour;
            minMin = ts->min - 15;
            maxMin = ts->min + 15;
            if (maxMin < 60) { /* 当前小时 */
                for (i = 0; i < n; i++) {
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
            } else { /* 当前+后1小时 */
                maxMin -= 60;
                maxHr = (minHr + 1) % MAX_HOUR_NUM;
                for (i = 0; i < n; i++) {
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
        } else {
            /* 前1小时+当前小时 */
            minHr = (ts->hour + MAX_HOUR_NUM - 1) % MAX_HOUR_NUM;
            minMin = ts->min + 60 - 15;
            maxMin = ts->min + 15;
            maxHr = ts->hour;
            for (i = 0; i < n; i++) {
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

