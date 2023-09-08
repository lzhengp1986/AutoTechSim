#ifndef WENV_H
#define WENV_H

#include <QList>
#include <QStringList>

/* DB频点信息 */
typedef struct {
    int freq; /* KHz */
    int mufday;
    int snr;
    int rel;
} DbFreq;

/* 频点数固定 */
#define MAX_FREQ_NUM 12
#define MAX_HOUR_NUM 24

/* DB月信息 */
typedef struct {
    struct {
        DbFreq fc[MAX_FREQ_NUM];
    } hr[MAX_HOUR_NUM];
} DbMonth;

class WEnv
{
public:
    WEnv(void);
    ~WEnv(void);

    /* 窗口api */
    int get_index(void) const;
    int get_month(void) const;
    int get_year(void) const;
    const QStringList& get_list(void) const;
    int setup(int year, int index, int month, const QString& db);

    /* 信道api */
    bool env(int year, int month, int hour, int glbChId, bool& flag, int& snr);

private:
    int m_index;
    int m_month;
    int m_year;
    DbMonth m_dbMonth;
    QStringList m_dbList;
};

/* inline */
inline int WEnv::get_index(void) const
{
    return m_index;
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
