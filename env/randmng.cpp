#include "randmng.h"
#include "sql/util.h"
#include <QRandomGenerator>
#include <QTime>

// 初始化种子
RandMng::RandMng(void)
{
    int msec = QTime::currentTime().msecsSinceStartOfDay();
    QRandomGenerator rnd((uint)msec);

    int i, j, k;
    for (i = 0; i < 2; i++) {
        for (j = 0; j <= MAX_GLB_CHN; j++) {
            k = rnd.bounded(INT32_MAX);
            m_gen[i][j] = new RandGen(k);
        }
    }
}

RandMng::~RandMng(void)
{
    int i, j;
    for (i = 0; i < 2; i++) {
        for (j = 0; j <= MAX_GLB_CHN; j++) {
            delete m_gen[i][j];
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
    UnifIntDist dist(a, b);
    return dist(*m_gen[0][glbChId]);
}

int RandMng::grn(int glbChId, double u, double g)
{
    /* 参数检查 */
    if ((glbChId < 0) || (glbChId >= MAX_GLB_CHN)) {
        glbChId = MAX_GLB_CHN;
    }

    /* 生成随机数 */
    NormDist dist(u, g);
    double data = dist(*m_gen[1][glbChId]);
    return (int)data;
}

