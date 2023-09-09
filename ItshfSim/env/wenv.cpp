#include "wenv.h"
#include "macro.h"
#include "sqlite3.h"
#include <QMessageBox>
#include <math.h>

// 构造
WEnv::WEnv(void)
{
    m_model.month = 1;
    m_model.year = 2023;
    m_model.dbDesc
            << "成都市区-乐山沐川"
            << "成都市区-陕西西安"
            << "成都市区-海南三亚";

    m_model.dbIndex = 0;
    m_model.bandIndex = 0;
    memset(&m_dbMonth, 0, sizeof(DbMonth));
}

// 析构
WEnv::~WEnv(void)
{
}

// 根据dialog选择的Model读出DB月份数据
int WEnv::setup(const ModelCfg& in, const QString& fn)
{
    m_model = in;

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
    QString sql = "SELECT * FROM ITU WHERE month=" + QString::number(in.month - 1);
    std::string stdsql = sql.toStdString();
    const char* cmd = stdsql.c_str();
    rc = sqlite3_prepare_v2(db, cmd, -1, &stmt, NULL);
    if (rc != SQLITE_OK ) {
        sqlite3_close(db);
        return 2;
    }

    /* 循环读数据 */
    int fid, prvHour = 0;
    int result = SQLITE_OK;
    int hour, freq, mufday, snr, rel;
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
        hour = sqlite3_column_int(stmt, 3);
        freq = sqlite3_column_int(stmt, 4);
        mufday = sqlite3_column_int(stmt, 5);
        snr = sqlite3_column_int(stmt, 7);
        rel = sqlite3_column_int(stmt, 8);

        /* hour值变化 */
        if (hour != prvHour) {
            prvHour = hour;
            fid = 0;
        } else {
            fid++;
        }

        /* 保存数据 */
        if ((hour <= MAX_HOUR_NUM) && (hour > 0)) {
            DbFreq* df = &m_dbMonth.hr[hour - 1].fc[fid];
            df->mufday = mufday;
            df->freq = freq;
            df->snr = snr;
            df->rel = rel;
        }
    }

    /* 关闭db */
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

// 参数检查
int WEnv::check(const EnvIn& in)
{
    /* 时间检查 */
    if ((in.year != m_model.year)
        || (in.month != m_model.month)) {
        return -1;
    }

    /* 有效性检查 */
    int hour = in.hour;
    int month = in.month;
    int glbChId = in.glbChId;
    if ((hour <= 0) || (hour > MAX_HOUR_NUM)
        || (month < 0) || (month > MAX_MONTH_NUM)
        || (glbChId < 0) || (glbChId > MAX_GLB_CHN)) {
        return -2;
    }

    return 0;
}

// api调度函数
int WEnv::env(const EnvIn& in, EnvOut& out)
{
    /* 初始化 */
    out.snr = MIN_SNR;
    out.flag = false;

    /* 时间检查 */
    bool ret = check(in);
    if (ret != 0) {
        return ret;
    }

    /* 计算可用标志 */
    bool flag = calc(in, out);
    return flag;
}

// 根据时戳和信道号结合Model计算可用标志和SNR估计值
bool WEnv::calc(const EnvIn& in, EnvOut& out)
{
    /* 初始化 */
    out.flag = false;
    out.snr = MIN_SNR;

    /* 获取Hour信息 */
    DbHour* dh = &m_dbMonth.hr[in.hour - 1];
    int maxband = get_maxband(m_model.bandIndex);

    /* 计算可通频率范围 */
    int muf = dh->fc[0].freq;
    int halfband = maxband / 2;
    int min = MAX(muf - halfband, MIN_CHN_FREQ);
    int max = MIN(muf + halfband, MAX_CHN_FREQ);

    /* 是否在可通频带 */
    int glbChId = in.glbChId;
    int fc = glb2freq(glbChId);
    if ((fc < min) || (fc > max)) {
        return false;
    }

    /* 找MUFday/SNR */
    int snr = dh->fc[0].snr;
    int mufday = dh->fc[0].mufday;
    for (int i = 1; i < MAX_FREQ_NUM; i++) {
        if (dh->fc[i].freq >= fc) {
            mufday = dh->fc[i].mufday;
            snr = dh->fc[i].snr;
        }
    }

    /* 随机数拟合MUFday */
    int rnd = m_rand.rab(glbChId, 0, 100);
    if (rnd > mufday) {
        return true;
    }

    /* 随机数拟合SNR */
    int u = (snr - 60) / 10;
    int g = 5 + fabs(snr - 50) / 10;
    int expSnr = m_rand.grn(glbChId, u, g);
    out.flag = (expSnr > MIN_SNR);
    out.snr = expSnr;
    return true;
}
