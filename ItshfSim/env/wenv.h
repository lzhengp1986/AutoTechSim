#ifndef WENV_H
#define WENV_H

#include <QList>
#include <QStringList>

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
