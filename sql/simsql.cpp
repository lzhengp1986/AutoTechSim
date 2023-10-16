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

char* SimSql::regMin(int tab, const Time* ts, int min)
{
    char* sql;
    int half = MIN(min, 59) >> 1;
    Time prv = subMin(ts, half);
    Time nxt = addMin(ts, half);
    const char* tlist[] = {"SCAN", "LINK"};

    if (prv.hour == nxt.hour) {
        /* 不跨小时 */
        sql = sqlite3_mprintf("select * from %s where year=%d and month=%d and hour=%d"
                              " and (min>=%d and min<=%d) order by snr desc", tlist[tab],
                              prv.year, prv.month, prv.hour, prv.min, nxt.min);
    } else {
        /* 2段定时 */
        sql = sqlite3_mprintf("select * from %s where year=%d and month=%d and ((hour==%d and min>=%d)"
                              " or (hour==%d and min<=%d)) order by snr desc", tlist[tab],
                              prv.year, prv.month, prv.hour, prv.min, nxt.hour, nxt.min);
    }

    return sql;
}

char* SimSql::regHour(int tab, const Time* ts, int hr)
{
    char* sql;
    int half = MAX(hr, 2) >> 1;
    Time prv = subHour(ts, half);
    Time nxt = addHour(ts, half);
    const char* tlist[] = {"SCAN", "LINK"};

    if (prv.day == nxt.day) {
        /* 不跨24小时，只有1段 */
        sql = sqlite3_mprintf("select * from %s where year=%d and month=%d"
                              " and (hour>=%d and hour<=%d) order by snr desc",
                              tlist[tab], prv.year, prv.month, prv.hour, nxt.hour);
    } else {
        /* 2段定时: min~24, 0~max */
        sql = sqlite3_mprintf("select * from %s where year=%d and month=%d"
                              " and (hour>=%d or hour<=%d) order by snr desc",
                              tlist[tab], prv.year, prv.month, prv.hour, nxt.hour);
    }

    return sql;
}

// 根据类型生成selec规则
char* SimSql::regular(int tab, const Time* ts, int rule)
{
    char* sql;
    switch (rule) {
    case SQL_WIN_30MIN: sql = regMin(tab, ts, 30); break;
    case SQL_WIN_1HOUR: sql = regMin(tab, ts, 60); break;
    case SQL_WIN_2HOUR: sql = regHour(tab, ts, 2); break;
    case SQL_WIN_4HOUR: sql = regHour(tab, ts, 4); break;
    default: sql = regMin(tab, ts, 60); break;
    }

    return sql;
}

// 减少x分钟
Time SimSql::subMin(const Time* ts, int min)
{
    Time prv = *ts;

    /* 足够抵消 */
    if (ts->min >= min) {
        prv.min = ts->min - min;
        return prv;
    }

    /* hour抵扣 */
    if (ts->hour > 0) {
        prv.hour--;
        prv.min = ts->min + 60 - min;
        return prv;
    }

    /* day抵扣 */
    if (ts->day > 1) {
        prv.day--;
        prv.hour = 23;
        prv.min = ts->min + 60 - min;
        return prv;
    }

    /* 回到原点 */
    prv.min = 0;
    return prv;
}

// 增加x分钟
Time SimSql::addMin(const Time* ts, int min)
{
    Time next = *ts;

    /* 不进位 */
    next.min = ts->min + min;
    if (next.min < 60) {
        return next;
    }
    next.min -= 60;
    next.hour++;

    /* hour进位 */
    if (next.hour < MAX_HOUR_NUM) {
        return next;
    }
    next.hour -= MAX_HOUR_NUM;
    next.day++;

    /* day进位 */
    int md = ts->mdays();
    if (next.day <= md) {
        return next;
    }

    /* 年月不进位 */
    next.day -= md;
    return next;
}

// 减少x小时
Time SimSql::subHour(const Time* ts, int hr)
{
    Time prv = *ts;

    /* 足够抵扣 */
    if (ts->hour > hr) {
        prv.hour = ts->hour - hr;
        return prv;
    }

    /* day抵扣 */
    if (ts->day > 1) {
        prv.day--;
        prv.hour = ts->hour + MAX_HOUR_NUM - hr;
        return prv;
    }

    /* 回到原点 */
    prv.hour = 0;
    return prv;
}

// 增加x小时
Time SimSql::addHour(const Time* ts, int hr)
{
    Time next = *ts;

    /* 不进位 */
    next.hour = ts->hour + hr;
    if (next.hour < MAX_HOUR_NUM) {
        return next;
    }
    next.hour -= MAX_HOUR_NUM;
    next.day++;

    /* day进位 */
    int md = ts->mdays();
    if (next.day <= md) {
        return next;
    }

    /* 年月不进位 */
    next.day -= md;
    return next;

}

// 筛选数据
int SimSql::select(int tab, const Time* ts, int rule, QList<FreqInfo>& list)
{
    list.clear();
    if (m_handle == nullptr) {
        printf("err: null DB handle\n");
        fflush(stdout);
        return SQLITE_ERROR;
    }

    /* 参数准备 */
    sqlite3_stmt* stmt;
    char* sql = regular(tab, ts, rule);
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
        info.hour = sqlite3_column_int(stmt, 3);
        info.min = sqlite3_column_int(stmt, 4);
        info.valid = sqlite3_column_int(stmt, 6);
        info.glbChId = sqlite3_column_int(stmt, 7);
        info.snr = sqlite3_column_int(stmt, 8);
        info.n0 = sqlite3_column_int(stmt, 9);
        info.isNew = true;

        /* 保存数据 */
        list.append(info);

        /* 样本过大 */
        if (list.size() >= MAX_SQL_SMPL) {
            break;
        }
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

