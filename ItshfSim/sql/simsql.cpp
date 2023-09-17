#include "simsql.h"
#include <stdio.h>

// 构造
SimSql::SimSql(void)
{
    sqlite3 *db;
    m_handle = nullptr;
    const char* path = "./png/sim.db";
    int ret = sqlite3_open_v2(path, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (ret != SQLITE_OK) {
        printf("err: open DB\n");
        fflush(stdout);
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
    if (m_handle == nullptr) {
        printf("err: null DB handle\n");
        fflush(stdout);
        return SQLITE_ERROR;
    }

    char* errMsg = nullptr;
    const char* tlist[] = {"SCAN", "LINK"};
    char* sql = sqlite3_mprintf("insert into %s values(%d,%d,%d,%d,%d, %d,%d,%d,%d,%d)", tlist[tab],
                                ts->year, ts->month, ts->day, ts->hour, ts->min, ts->sec,
                                valid, glbChId, snr, noise);
    int rc = sqlite3_exec(m_handle, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("err: insert %s failure %s\n", tlist[tab], errMsg);
        sqlite3_free(errMsg);
        sqlite3_free(sql);
        fflush(stdout);
        return rc;
    }
    sqlite3_free(sql);
    return SQLITE_OK;
}

// 筛选数据
int SimSql::select(int tab, const Time* ts, int min, QList<FreqInfo>& list)
{
    list.clear();
    if (m_handle == nullptr) {
        printf("err: null DB handle\n");
        fflush(stdout);
        return SQLITE_ERROR;
    }

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
    char* sql;
    const char* tlist[] = {"SCAN", "LINK"};
    if (minDay == maxDay) { /* 同一天 */
        sql = sqlite3_mprintf("select * from %s where year=%d and month=%d and day=%d"
                              " and ((hour=%d and min>=%d) or (hour>%d and hour<%d)" /* 最早+中间 */
                              " or (hour=%d)) order by snr desc",  /* 最新 */
                              tlist[tab], ts->year, ts->month, ts->day,
                              minHr, minMin, minHr, maxHr, maxHr);
    } else if (minDay + 1 == maxDay) { /* 相临两天 */
        sql = sqlite3_mprintf("select * from %s where year=%d and month=%d"
                              " and ((day=%d and (hour>%d or (hour=%d and min>=%d)))" /* 最早 */
                              " or (day=%d)) order by snr desc",  /* 最新 */
                              tlist[tab], ts->year, ts->month,
                              minDay, minHr, minHr, minMin,
                              maxDay);
    } else { /* 相临几天 */
        sql = sqlite3_mprintf("select * from %s where year=%d and month=%d"
                              " and ((day=%d and hour>=%d) or (day>%d))"
                              " order by snr desc",
                              tlist[tab], ts->year, ts->month,
                              minDay, minHr, minDay);
    }

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_handle, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK ) {
        sqlite3_free(sql);
        sqlite3_close(m_handle);
        return rc;
    }
    sqlite3_free(sql);

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
    if (m_handle == nullptr) {
        printf("err: null DB handle\n");
        fflush(stdout);
        return;
    }

    /* 删除表格 */
    char* errMsg = nullptr;
    const char* tlist[] = {"SCAN", "LINK"};
    char* sql1 = sqlite3_mprintf("drop table if exists %s", tlist[tab]);
    int rc = sqlite3_exec(m_handle, sql1, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("err: drop %s failure %s\n", tlist[tab], errMsg);
        sqlite3_free(errMsg);
        sqlite3_free(sql1);
        fflush(stdout);
        return;
    }
    sqlite3_free(sql1);

    /* 重建表格 */
    char *sql2 = sqlite3_mprintf("create table if not exists %s(year INTEGER, month INTEGER, day INTEGER, hour INTEGER,"
                                 "min INTEGER, sec INTEGER, valid INTEGER, glbChId INTEGER, snr INTEGER, noise INTEGER,"
                                 "UNIQUE(year, month, day, hour, min, sec, glbChId) ON CONFLICT REPLACE)", tlist[tab]);
    rc = sqlite3_exec(m_handle, sql2, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        printf("err: create SCAN failure %s\n", errMsg);
        sqlite3_free(errMsg);
        sqlite3_free(sql2);
        return;
    }
    sqlite3_free(sql2);

    /* 释放内存 */
    sqlite3_exec(m_handle, "VACUUM", 0, 0, NULL);
}

