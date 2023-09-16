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

// 析构
SimSql::~SimSql(void)
{
    sqlite3_close(m_handle);
    m_handle = nullptr;
}

// 插入数据
int SimSql::insert(int tab, const Time* ts, int valid, int glbChId, int snr, int noise)
{
    char* errMsg = nullptr;
    const char* tlist[] = {"SCAN", "LINK"};
    char* sql = sqlite3_mprintf("insert into %s values(%d,%d,%d,%d,%d, %d,%d,%d,%d,%d)", tlist[tab],
                                ts->year, ts->month, ts->day, ts->hour, ts->min, ts->sec,
                                valid, glbChId, snr, noise);
    int rc = sqlite3_exec(m_handle, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("err: insert %s failure %s\n", tlist[tab], errMsg);
        sqlite3_free(errMsg);
        return rc;
    }
    sqlite3_free(sql);
    return SQLITE_OK;
}

// 筛选数据
int SimSql::select(int tab, const Time* ts, int min, QList<FreqInfo>& list)
{
    int minDay, minHr, minMin;
    int maxDay, maxHr, maxMin;

    /* 计算定时 */
    maxDay = minDay = ts->day;
    maxHr = minHr = ts->hour;
    maxMin = minMin = ts->min;
    while (minMin < min) {
        minMin += 60;
        if (minHr > 0) {
            minHr--;
        } else {
            minHr += 23;
            minDay--;
        }
    }
    minMin -= min;

    /* 参数准备 */
    sqlite3_stmt* stmt;
    const char* tlist[] = {"SCAN", "LINK"};
    char* sql = sqlite3_mprintf("select * from %s where year=%d and month=%d"
                                " and (day=%d and hour>%d or (hour=%d and min>=%d))" /* 最早时刻 */
                                " and (day=%d and hour<%d or (hour=%d and min<=%d))" /* 最新时刻 */
                                " order by snr desc", tlist[tab], ts->year, ts->month,
                                minDay, minHr, minHr, minMin, maxDay, maxHr, maxHr, maxMin);
    int rc = sqlite3_prepare_v2(m_handle, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK ) {
        sqlite3_close(m_handle);
        return rc;
    }

    /* 循环读数据 */
    FreqInfo info;
    int result = SQLITE_OK;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
        info.valid = sqlite3_column_int(stmt, 6);
        info.glbChId = sqlite3_column_int(stmt, 7);
        info.snr = sqlite3_column_int(stmt, 8);
        info.n0 = sqlite3_column_int(stmt, 9);

        /* 保存数据 */
        list.append(info);
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

// 丢弃所有数据
void SimSql::drop(int tab)
{
    char* errMsg = nullptr;
    const char* tlist[] = {"SCAN", "LINK"};
    char* sql = sqlite3_mprintf("drop table if exists %s", tlist[tab]);
    int rc = sqlite3_exec(m_handle, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("err: drop %s failure %s\n", tlist[tab], errMsg);
        sqlite3_free(errMsg);
    }

    /* 释放内存 */
    sqlite3_exec(m_handle, "VACUUM", 0, 0, NULL);
}

