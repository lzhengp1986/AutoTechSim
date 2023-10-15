#ifndef RANDMNG_H
#define RANDMNG_H

#include "sql/util.h"
#include "macro.h"

class RandMng {
public:
    RandMng(void);
    ~RandMng(void);

    /* api */
    int rab(int glbChId, int a, int b);
    int grn(int glbChId, int u, int g);

private:
    RandGen *m_gen[2][MAX_GLB_CHN + 1];
};

#endif // RANDMNG_H
