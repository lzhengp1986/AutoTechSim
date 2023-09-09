#ifndef WENV_H
#define WENV_H

//#include "wrand.h"
#include "model.h"
#include <QStringList>

/* 入参 */
typedef struct {
    int year;
    int month; /* 1~12 */
    int hour; /* 1~24 */
    int glbChId; /* 0~9333 */
} EnvIn;

/* 出参 */
typedef struct {
    bool flag;
    int snr;
} EnvOut;

/* DB频点信息 */
typedef struct {
    int freq; /* KHz */
    int mufday;
    int snr;
    int rel;
} DbFreq;

/* 频点数固定 */
#define MAX_FREQ_NUM 12
#define MAX_MONTH_NUM 12
#define MAX_HOUR_NUM 24

/* hour信息 */
typedef struct {
    DbFreq fc[MAX_FREQ_NUM];
} DbHour;

/* DB月信息 */
typedef struct {
    DbHour hr[MAX_HOUR_NUM];
} DbMonth;

class WEnv
{
public:
    WEnv(void);
    ~WEnv(void);

    /* api */
    int get_maxband(void) const;
    const ModelCfg& get_model(void) const;

    /*! @brief 根据dialog选择的Model读出DB月份数据 */
    int setup(const ModelCfg& in, const QString& fn);

    /*! @brief 根据时戳和信道号结合Model计算可用标志和SNR估计值 */
    int env(const EnvIn& in, EnvOut& out);

private:
    int check(const EnvIn& in);
    bool calc(const EnvIn& in, EnvOut& out);

private:
    ModelCfg m_model;

    //WRand m_rand;
    DbMonth m_dbMonth;
};

/* inline */
inline const ModelCfg& WEnv::get_model(void) const
{
    return m_model;
}

inline int WEnv::get_maxband(void) const
{
    int maxband = 0;
    switch (m_model.bandIndex) {
    case 1: maxband = 1000; break;
    case 2: maxband = 2000; break;
    case 3: maxband = 4000; break;
    case 4: maxband = 8000; break;
    default: maxband = 30000; break;
    }

    return maxband;
}

#endif // WENV_H
