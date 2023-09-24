#include "randmng.h"
#include "sql/util.h"
#include <QTime>

// 初始化种子
RandMng::RandMng(void)
{
    int msec = QTime::currentTime().msecsSinceStartOfDay();
    qsrand((uint)msec);

    int i, j;
    for (i = 0; i <= MAX_GLB_CHN; i++) {
        j = qrand();
        m_fseed[i] = qrand();
        if ((j <= 0) || ((j & 0x1) == 0)) {
            m_iseed[i] = j + 1;
        } else {
            m_iseed[i] = j;
        }
    }
}

int RandMng::rab(int glbChId, int a, int b)
{
    /* 参数检查 */
    if ((glbChId < 0) || (glbChId >= MAX_GLB_CHN)) {
        glbChId = MAX_GLB_CHN;
    }

    /* 生成随机数 */
    int seed = m_iseed[glbChId];
    int data = rab1(a, b, &seed);
    m_iseed[glbChId] = seed;
    return data;
}

int RandMng::grn(int glbChId, int u, int g)
{
    /* 参数检查 */
    if ((glbChId < 0) || (glbChId >= MAX_GLB_CHN)) {
        glbChId = MAX_GLB_CHN;
    }

    /* 生成随机数 */
    double seed = m_fseed[glbChId];
    double data = grn1(u, g, &seed);
    m_fseed[glbChId] = seed;
    return (int)data;
}

