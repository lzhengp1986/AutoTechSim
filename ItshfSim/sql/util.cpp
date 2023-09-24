#include <math.h>

/* X样本个数 */
#define BETA_SMPL_NUM 128

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

// 生成1个gama随机数
static double gam1(double x)
{
    int i;
    double y,t,s,u;
    static double a[11]={ +0.0000677106,-0.0003442342,
                          +0.0015397681,-0.0024467480,+0.0109736958,
                          -0.0002109075,+0.0742379071,+0.0815782188,
                          +0.4118402518,+0.4227843370,+1.0};
    if (x <= 0.0) {
        return(-1.0);
    }

    y = x;
    if (y <= 1.0) {
        t = 1.0 / (y * (y + 1.0));
        y = y + 2.0;
    } else if (y <= 2.0) {
        t = 1.0 / y;
        y = y + 1.0;
    } else if (y <= 3.0) {
        t = 1.0;
    } else {
        t = 1.0;
        while (y > 3.0) {
            y = y - 1.0;
            t = t * y;
        }
    }

    s = a[0];
    u = y - 2.0;
    for (i = 1; i <= 10; i++) {
        s = s * u + a[i];
    }

    s = s * t;
    return s;
}

static double bt(double a, double b, double x)
{
    int k;
    double d, s0, s1;

    double p0 = 0.0;
    double q0 = 1.0;
    double p1 = 1.0;
    double q1 = 1.0;

    for (k = 1; k <= 100; k++) {
        d = (a + k) * (a + b + k) * x;
        d = -d / ((a + k + k) * (a + k + k + 1.0));
        p0 = p1 + d * p0;
        q0 = q1 + d * q0;
        s0 = p0 / q0;
        d = k * (b - k) * x;
        d = d / ((a + k + k - 1.0) * (a + k + k));
        p1 = p0 + d * p1;
        q1 = q0 + d * q1;
        s1 = p1 / q1;
        if (fabs(s1 - s0) < fabs(s1) * 1.0e-07) {
            return s1;
        }
    }

    return(s1);
}

// 计算beta函数值
double beta(double a, double b, double x)
{
    double y;

    if (a <= 0.0) {
        return 0.0;
    }

    if (b <= 0.0) {
        return 1.0;
    }

    if ((x < 0.0) || (x > 1.0)) {
        return 1.0e+70;
    }

    if ((x == 0.0)||(x == 1.0)) {
        y = 0.0;
    } else {
        y = a * log(x) + b * log(1.0-x);
        y = exp(y);
        y = y * gam1(a + b) / (gam1(a) * gam1(b));
    }

    if (x < (a + 1.0) / (a + b + 2.0)) {
        y = y * bt(a, b, x) / a;
    } else {
        y = 1.0 - y * bt(b, a, 1.0 - x) / b;
    }

    return y;
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

// 生成1个beta分布随机数
double rbeta(double a, double b, double *r)
{
    int i, j;
    double x, y;

    /* 对概率函数采样 */
    double pr[BETA_SMPL_NUM] = {0};
    for (i = 1; i < BETA_SMPL_NUM; i ++) {
        x = (double)i / BETA_SMPL_NUM;
        y = beta(a, b, x);
        pr[i] = pr[i - 1] + y;
    }

    /* 产生均匀分布随机数 */
    double sum = pr[BETA_SMPL_NUM - 1];
    double rx = rnd1(r) * sum;

    /* 判断r所在区间 */
    if (rx <= 0) {
        j = 0;
    } else if (rx >= sum) {
        j = BETA_SMPL_NUM - 1;
    } else {
        for (i = j = BETA_SMPL_NUM - 1; i > 0; i--) {
            if (rx >= pr[i]) {
                j = i;
                break;
            }
        }
    }

    /* 计算x值 */
    x = (double)j / BETA_SMPL_NUM;
    return x;
}

