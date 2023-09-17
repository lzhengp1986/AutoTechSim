#ifndef SIMSQL_H
#define SIMSQL_H

#include "type.h"
#include "sqlite3.h"
#include <QList>

// 频点信息
typedef struct {
    int glbChId;
    int valid;
    int snr;
    int n0;
} FreqInfo;

// 仿真SQL
class SimSql
{
public:
    SimSql(void);
    ~SimSql(void);

    /* SQL操作 */
    /*! @brief 插入信道样本 */
    int insert(int tab, const Time* ts, int valid, int glbChId, int snr, int noise);
    /*! @brief 筛选历史记录，min表示往前多少分钟 */
    int select(int tab, const Time* ts, int min, QList<FreqInfo>& list);
    /*! @brief 丢弃所有数据 */
    void drop(int tab);

private:
    sqlite3* m_handle;
};

// sql入参
class SqlIn {
public:
    SqlIn(const Time* ts = nullptr, SimSql* sql = nullptr, int min = 60);

public:
    int sqlMin; /* sql时段 */
    SimSql* mysql; /* sql句柄 */
    const Time* stamp; /* 时戳 */
};

inline SqlIn::SqlIn(const Time* ts, SimSql* sql, int min)
{
    stamp = ts;
    mysql = sql;
    sqlMin = min;
}

#endif // SIMSQL_H
