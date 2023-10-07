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

// 重新找中心点
void BisectAlg::restart(SqlIn& in)
{
    /* 找最好的中心 */
    int optChId;
    bool flag = best(in, optChId);
    if (flag == true) {
        m_prvGlbChId = optChId;
    }

    /* 清状态 */
    memset(m_valid, 0, sizeof(m_valid));
    m_valid[m_prvGlbChId] = true;
    m_firstStage = true;
}

const FreqRsp& BisectAlg::bandit(SqlIn& in, const FreqReq& req)
{
    Q_UNUSED(in);

    FreqRsp* rsp = &m_rsp;
    int n = MIN(req.fcNum, RSP_FREQ_NUM);

    /* 300KHz附近选点 */
    rsp->glb[0] = chId300K(m_prvGlbChId);
    rsp->glb[1] = align(m_prvGlbChId);

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

    /* 将二分频点提前 */
    int rnd = rab1(0, 99, &m_seedi);
    if (rnd < 40) {
        int tmp1 = rsp->glb[3];
        rsp->glb[3] = rsp->glb[1];
        rsp->glb[1] = tmp1;
        int tmp2 = rsp->glb[2];
        rsp->glb[2] = rsp->glb[0];
        rsp->glb[0] = tmp2;
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

    /* 获取历史最优 */
    int optChId;
    bool flag = best(in, optChId);
    if (flag == true) {
        m_prvGlbChId = optChId;
        m_valid[m_prvGlbChId] = true;
    }

    /* 捕获成功切状态 */
    if (out.isValid == true) {
        memset(m_valid, 0, sizeof(m_valid));
        m_valid[m_prvGlbChId] = true;
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

// 找最好的中心
bool BisectAlg::best(SqlIn& in, int& optChId)
{
    bool flag = false;

    /* 读取历史记录 */
    in.mysql->select(SMPL_LINK, in.stamp, in.myRule, m_sqlList);
    int n = m_sqlList.size();
    if (n <= 0) {
        return flag;
    }

    /* 样本清零 */
    int i, glbChId;
    for (i = 0; i < n; i++) {
        glbChId = m_sqlList.at(i).glbChId;
        m_snrSum[glbChId] = 0;
        m_snrNum[glbChId] = 0;
        m_vldNum[glbChId] = 0;
        m_invNum[glbChId] = 0;
    }

    /* 样本统计 */
    bool valid;
    for (i = 0; i < n; i++) {
        const FreqInfo& info = m_sqlList.at(i);
        glbChId = info.glbChId;
        valid = info.valid;

        /* SNR统计 */
        m_snrSum[glbChId] += info.snr;
        m_snrNum[glbChId] ++;

        /* thompson统计 */
        m_vldNum[glbChId] += (valid == true);
        m_invNum[glbChId] += (valid == false);
    }

    /* 最大均值 */
    int maxIdx = 0;
    float maxAvg = INV_SNR;
    for (i = 0; i < n; i++) {
        glbChId = m_sqlList.at(i).glbChId;
        float snr = avgSnr(glbChId);
        if (snr > maxAvg) {
            maxIdx = glbChId;
            maxAvg = snr;
            flag = true;
        }
    }

    optChId = maxIdx;
    return flag;
}

// 平均snr
float BisectAlg::avgSnr(int i)
{
    float avg = 0;
    if (m_snrNum[i] <= 0) {
        return avg;
    }

    avg = (float)m_snrSum[i] / m_snrNum[i];
    return avg;
}

