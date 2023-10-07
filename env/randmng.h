#ifndef RANDMNG_H
#define RANDMNG_H

#include "macro.h"

class RandMng {
public:
    RandMng(void);

    /* api */
    int rab(int glbChId, int a, int b);
    int grn(int glbChId, int u, int g);

private:
    int m_iseed[MAX_GLB_CHN + 2];
    double m_fseed[MAX_GLB_CHN + 2];

};

#endif // RANDMNG_H
