#ifndef WENV_H
#define WENV_H

//#include "wrand.h"
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
    int get_year(void) const;
    int get_month(void) const;
    int get_dbIndex(void) const;
    int get_bandIndex(void) const;
    int get_maxband(void) const;

    /*! @brief 向dialog提供可用的DB Model列表 */
    const QStringList& get_list(void) const;

    /*! @brief 根据dialog选择的Model读出DB月份数据 */
    int setup(int year, int month, const QString& db, int dbIndex, int bandIndex);

    /*! @brief 根据时戳和信道号结合Model计算可用标志和SNR估计值 */
    int env(const EnvIn& in, EnvOut& out);

private:
    int check(const EnvIn& in);
    bool calc(const EnvIn& in, EnvOut& out);

private:
    int m_year;
    int m_month;
    int m_dbIndex;
    int m_bandIndex;

    //WRand m_rand;
    DbMonth m_dbMonth;
    QStringList m_dbList;
};

/* inline */
inline int WEnv::get_bandIndex(void) const
{
    return m_bandIndex;
}

inline int WEnv::get_maxband(void) const
{
    int maxband = 0;
    switch (m_bandIndex) {
    case 1: maxband = 1000; break;
    case 2: maxband = 2000; break;
    case 3: maxband = 4000; break;
    case 4: maxband = 8000; break;
    default: maxband = 30000; break;
    }

    return maxband;
}

inline int WEnv::get_dbIndex(void) const
{
    return m_dbIndex;
}

inline int WEnv::get_month(void) const
{
    return m_month;
}

inline int WEnv::get_year(void) const
{
    return m_year;
}

inline const QStringList& WEnv::get_list(void) const
{
    return m_dbList;
}

#endif // WENV_H
