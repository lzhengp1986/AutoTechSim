#ifndef SIMSQL_H
#define SIMSQL_H

#include "type.h"
#include "sqlite3.h"
#include <QList>

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
    int select(int tab, const Time* ts, int rule, QList<FreqInfo>& list);
    /*! @brief 丢弃所有数据 */
    void drop(int tab);

private:
    /*! @brief 根据类型生成selec规则 */
    char* regular(int tab, const Time* ts, int rule);
    char* fwdMin(int tab, const Time* ts, int min);
    char* fwdHour(int tab, const Time* ts, int hr);

private:
    sqlite3* m_handle;
};

// sql入参
class SqlIn {
public:
    SqlIn(const Time* ts = nullptr, SimSql* sql = nullptr, int rule = 0);

public:
    int myRule; /* 时段规则 */
    SimSql* mysql; /* sql句柄 */
    const Time* stamp; /* 时戳 */
};

inline SqlIn::SqlIn(const Time* ts, SimSql* sql, int rule)
{
    stamp = ts;
    mysql = sql;
    myRule = rule;
}

#endif // SIMSQL_H
