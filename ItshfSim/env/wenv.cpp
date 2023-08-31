#include "wenv.h"
#include "sqlite3.h"
#include <QMessageBox>

// 构造
WEnv::WEnv(void)
{
    m_index = 0;
    m_month = 1;
    m_dbList << "2023-成都市区-乐山沐川"
             << "2023-成都市区-陕西西安"
             << "2023-成都市区-海南三亚";
}

// 析构
WEnv::~WEnv(void)
{
}

// 数据库
int WEnv::setup(int index, int month, const QString& fn)
{
    m_index = index;
    m_month = month;

    /* 打开db */
    sqlite3* db = nullptr;
    std::string stdfn = fn.toStdString();
    const char* file = stdfn.c_str();
    int rc = sqlite3_open_v2(file, &db, SQLITE_OPEN_READONLY, NULL);
    if (rc != SQLITE_OK) {
        return 1;
    }

    /* 参数准备 */
    sqlite3_stmt* stmt;
    QString sql = "SELECT * FROM ITU WHERE month=" + QString::number(month - 1);
    std::string stdsql = sql.toStdString();
    const char* cmd = stdsql.c_str();
    rc = sqlite3_prepare_v2(db, cmd, -1, &stmt, NULL);
    if (rc != SQLITE_OK ) {
        sqlite3_close(db);
        return 2;
    }

    /* 循环读数据 */
    int result = SQLITE_OK;
    int hour, freq, mufday, snr, rel;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
        hour = sqlite3_column_int(stmt, 3);
        freq = sqlite3_column_int(stmt, 4);
        mufday = sqlite3_column_int(stmt, 5);
        snr = sqlite3_column_int(stmt, 7);
        rel = sqlite3_column_int(stmt, 8);
    }

    /* 关闭db */
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}
