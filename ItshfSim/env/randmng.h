#ifndef RANDMNG_H
#define RANDMNG_H

#include "macro.h"

class Randi {
public:
    Randi(void);

    /*! @brief 产生均匀随机数 */
    int rab(int a, int b);

private:
    int m_seed;

};

class RandMng {
public:
    RandMng(void);

    /* api */
    int rab(int glbChId, int a, int b);
    int grn(int glbChId, int u, int g);

private:
    /*! @brief 参考《常用算法程序集》 */
    int rab1(int a, int b, int *r);
    double grn1(double u, double g, double *r);

private:
    int m_iseed[MAX_GLB_CHN + 2];
    double m_fseed[MAX_GLB_CHN + 2];

};

#endif // RANDMNG_H
