#include <math.h>
#include <stdio.h>

static double gam1(double x)
{
    int i;
    double y,t,s,u;
    static double a[11]={ 0.0000677106,-0.0003442342,
                          0.0015397681,-0.0024467480,0.0109736958,
                          -0.0002109075,0.0742379071,0.0815782188,
                          0.4118402518,0.4227843370,1.0};
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

    printf("a or b too big !");
    return(s1);
}

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
        y=1.0-y*bt(b, a, 1.0 - x) / b;
    }

    return(y);
}
