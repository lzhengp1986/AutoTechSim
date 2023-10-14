#include <math.h>
#include "boost/math/distributions.hpp"

// 只生成1个随机数
int rab1(int a, int b, int *r)
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
double grn1(double u, double g, double *r)
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

// 生成1个[0,1]均匀分布
double rnd1(double *r)
{
    int m;
    double s,u,v,p;
    s=65536.0;
    u=2053.0;
    v=13849.0;

    m = (int)(*r / s);
    *r = *r - m * s;
    *r = u * (*r) + v;
    m = (int)(*r / s);
    *r = *r - m * s;
    p = *r / s;
    return p;
}

