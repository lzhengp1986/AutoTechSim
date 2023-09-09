#ifndef WENV_H
#define WENV_H

#include "model.h"
#include "randmng.h"
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
    const ModelCfg& get_model(void) const;

    /*! @brief 根据dialog选择的Model读出DB月份数据 */
    int setup(const ModelCfg& in, const QString& fn);

    /*! @brief 根据时戳和信道号结合Model计算可用标志和SNR估计值 */
    int env(const EnvIn& in, EnvOut& out);

private:
    int check(const EnvIn& in);
    bool calc(const EnvIn& in, EnvOut& out);
    static int get_maxband(int bandIndex);
    static int glb2freq(int glbChId);

private:
    ModelCfg m_model; /* 模型参数 */
    DbMonth m_dbMonth; /* 月DB模型 */
    RandMng m_rand; /* 随机数发生器 */
};

/* inline */
inline const ModelCfg& WEnv::get_model(void) const
{
    return m_model;
}

inline int WEnv::get_maxband(int bandIndex)
{
    int maxband = 0;
    switch (bandIndex) {
    case 1: maxband = 1000; break;
    case 2: maxband = 2000; break;
    case 3: maxband = 4000; break;
    case 4: maxband = 8000; break;
    default: maxband = 30000; break;
    }

    return maxband;
}

inline int WEnv::glb2freq(int glbChId)
{
    return (glbChId * ONE_CHN_BW + MIN_CHN_FREQ);
}

#endif // WENV_H
