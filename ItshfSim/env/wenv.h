#ifndef WENV_H
#define WENV_H

#include "type.h"
#include "macro.h"
#include "modeldlg.h"
#include "randmng.h"
#include <QStringList>

/* 入参 */
class EnvIn {
public:
    EnvIn(const Time* ts, int glb);
    EnvIn(int y = 0, int m = 0, int h = 0, int glb = 0);

public:
    int year;
    int month; /* 1~12 */
    int hour; /* 1~24 */
    int glbChId; /* 0~9333 */
};

inline EnvIn::EnvIn(const Time* ts, int glb)
{
    year = ts->year;
    month = ts->month;
    hour = ts->hour;
    glbChId = glb;
}

inline EnvIn::EnvIn(int y, int m, int h, int glb)
{
    year = y;
    month = m;
    hour = h;
    glbChId = glb;
}

/* 出参 */
typedef struct {
    bool isValid; /* 可用标志 */
    int snr; /* 预估SNR */
    int n0; /* 预估底噪 */

    /* 策略评估 */
    bool mufVld; /* 最优标志 */
    int mufSnr; /* 最优SNR */
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
#define ENV_PROB_LO  3 /* 可用概率太低 */

/* 底噪数据长度 */
#define NOISE_NUM 640

/* grn函数的入参模型 */
#define GRN_U(snr) (((snr) - 60) / 10)
#define GRN_G(snr) (5 + ABS((snr) - 50) / 10)

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

    /*! @brief 根据信道号获取底噪 */
    static int noise(int glbChId);

    /*! @brief 获取所有底噪数据 */
    static const int* noise(void);

private:
    int check(const EnvIn& in);
    int estimate(const EnvIn& in, EnvOut& out);

private:
    int m_maxband; /* 最大可通带宽 */
    DbMonth m_dbMonth; /* 月DB模型 */
    RandMng m_frqRnd; /* 使用频点的随机数发生器 */
    RandMng m_mufRnd; /* MUF的随机数发生器 */
};

#endif // WENV_H
