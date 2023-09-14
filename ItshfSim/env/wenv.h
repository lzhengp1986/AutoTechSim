#ifndef WENV_H
#define WENV_H

#include "modeldlg.h"
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
    int muf;
    int snr;
    int n0;
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

/* 错误码 */
#define ENV_OK 0
#define ENV_INV_PARA 1 /* 参数非法 */
#define ENV_INV_GLB  2 /* glbChId非法 */

class WEnv
{
public:
    WEnv(void);
    ~WEnv(void);

    /* api */
    /*! @brief 根据dialog选择的Model读出DB月份数据 */
    int setup(int month, int maxband, const QString& fn);

    /*! @brief 根据时戳和信道号结合Model计算可用标志和SNR估计值 */
    int env(const EnvIn& in, EnvOut& out);

private:
    int check(const EnvIn& in);
    int calc(const EnvIn& in, EnvOut& out);

private:
    int m_maxband; /* 最大可通带宽 */
    DbMonth m_dbMonth; /* 月DB模型 */
    RandMng m_rand; /* 随机数发生器 */
};

#endif // WENV_H
