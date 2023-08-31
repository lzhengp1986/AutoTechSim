#include "wenv.h"

// 构造
WEnv::WEnv(void)
{
    m_index = 0;
    m_month = 1;
    m_db = nullptr;
    m_dbList << "2023-成都市区-乐山沐川"
             << "2023-成都市区-陕西西安"
             << "2023-成都市区-海南三亚";
}

// 析构
WEnv::~WEnv(void)
{
    if (m_db != nullptr) {
        sqlite3_close(m_db);
    }
}

// 数据库
void WEnv::setup(int index, int month, const QString& db)
{
    m_index = index;
    m_month = month;

    /* 打开db */
    if (m_db != nullptr) {
        sqlite3_close(m_db);
    }

}
