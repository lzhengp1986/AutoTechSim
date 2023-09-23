#include "randmng.h"
#include <QTime>

// 初始化种子
Randi::Randi(void)
{
    int ms = QTime::currentTime().msecsSinceStartOfDay();
    m_seed = ABS(ms) + ((ms & 0x1) == 0) + (ms > 0);
}

// 产生[a,b]均匀随机数
int Randi::rab(int a, int b)
{
    int k, l, m, i, p;
    k = b - a + 1;
    l = 2;
    while (l < k) {
        l = l + l;
    }

    m = 4 * l;
    k = m_seed;

    i = 1;
    while (i <= 1) {
        k = k+k+k+k+k;
        k = k % m;
        l = k / 4 + a;
        if (l <= b) {
            p = l;
            i = i + 1;
        }
    }

    m_seed = k;
    return p;
}

RandMng::RandMng(void)
{
    /* 初始化种子 */
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

// 只生成1个随机数
int RandMng::rab1(int a, int b, int *r)
{
    int k, l, m, i, p;
    k = b - a + 1;
    l = 2;
    while (l < k) {
        l = l + l;
    }

    m = 4 * l;
    k = *r;

    i = 1;
    while (i <= 1) {
        k = k+k+k+k+k;
        k = k % m;
        l = k / 4 + a;
        if (l <= b) {
            p = l;
            i = i + 1;
        }
    }

    *r = k;
    return(p);
}

// 生成1个正态分布
double RandMng::grn1(double u, double g, double *r)
{
    double s = 65536.0;
    double w = 2053.0;
    double v = 13849.0;

    int i, m;
    double t = 0.0;
    for (i = 1; i <= 12; i++) {
        *r = (*r) * w + v;
        m = (int)(*r / s);
        *r = *r - m * s;
        t = t + (*r) / s;
    }

    t = u + g * (t - 6.0);
    return t;
}
