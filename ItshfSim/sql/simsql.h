#ifndef SIMSQL_H
#define SIMSQL_H

#include "sqlite3.h"

// 仿真SQL
class SimSql
{
public:
    SimSql(void);

private:
    sqlite3* m_handle;
};

#endif // SIMSQL_H
