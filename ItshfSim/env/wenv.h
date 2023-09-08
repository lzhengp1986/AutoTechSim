#ifndef WENV_H
#define WENV_H

#include <QList>
#include <QStringList>

/* DB频点信息 */
typedef struct {
    int freq; /* KHz */
    int snr; /* 整数 */
    float mufday;
    float rel;
} DbFreq;

/* 频点数固定 */
#define MAX_FREQ_NUM 12
#define MAX_HOUR_NUM 24

/* DB月信息 */
typedef struct {
    DbFreq item[MAX_HOUR_NUM][MAX_FREQ_NUM];
} DbMonth;

class WEnv
{
public:
    WEnv(void);
    ~WEnv(void);

    /* api */
    int get_index(void) const;
    int get_month(void) const;
    const QStringList& get_list(void) const;
    int setup(int index, int month, const QString& db);

private:
    int m_index;
    int m_month;
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

inline const QStringList& WEnv::get_list(void) const
{
    return m_dbList;
}

#endif // WENV_H
