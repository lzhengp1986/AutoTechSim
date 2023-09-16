#include "simsql.h"
#include <stdio.h>

// 构造
SimSql::SimSql(void)
{
    /* 打开DB */
    sqlite3* db;
    const char* file = "sim.db";
    int rc = sqlite3_open_v2(file, &db, SQLITE_OPEN_READONLY, NULL);
    if (rc != SQLITE_OK) {
        m_handle = nullptr;
        return;
    }

    /* 保存句柄 */
    m_handle = db;
}


// 插入数据
