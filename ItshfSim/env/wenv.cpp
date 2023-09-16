#include "wenv.h"
#include "sqlite3.h"
#include <QMessageBox>

// 构造
WEnv::WEnv(void)
{
    m_maxband = 30000;
    memset(&m_dbMonth, 0, sizeof(DbMonth));
}

// 析构
WEnv::~WEnv(void)
{
}

// 根据dialog选择的Model读出DB月份数据
int WEnv::setup(int month, int maxband, const QString& fn)
{
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
    QString sql = "SELECT * FROM ITU WHERE month=" + QString::number(month);
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
    m_maxband = maxband;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

// 参数检查
int WEnv::check(const EnvIn& in)
{
    /* 有效性检查 */
    int hour = in.hour;
    int month = in.month;
    int glbChId = in.glbChId;
    if ((hour < 0) || (hour >= MAX_HOUR_NUM)
        || (month < 0) || (month > MAX_MONTH_NUM)
        || (glbChId < 0) || (glbChId >= MAX_GLB_CHN)) {
        return ENV_INV_PARA; /* 参数非法 */
    }

    return ENV_OK;
}

// api调度函数
int WEnv::env(const EnvIn& in, EnvOut& out)
{
    /* 初始化 */
    out.isValid = false;
    out.mufVld = false;
    out.snr = MIN_SNR - 1;
    out.mufSnr = MIN_SNR - 1;
    out.n0 = MIN_PN0;

    /* 时间检查 */
    int ret = check(in);
    if (ret != ENV_OK) {
        return ret;
    }

    /* 计算可用标志 */
    int flag = estimate(in, out);
    return flag;
}

// 根据时戳和信道号结合Model计算可用标志和SNR估计值
int WEnv::estimate(const EnvIn& in, EnvOut& out)
{
    /* 获取底噪 */
    const int* pnoise = noise();
    int glbChId = in.glbChId;
    int locChId = glbChId / CHN_SCAN_STEP;
    int subId = locChId / 3; /* 3个信道压缩 */
    out.n0 = pnoise[subId];

    /* 获取Hour信息 */
    DbHour* dh = &m_dbMonth.hr[in.hour];

    /* 计算可通频率范围 */
    int muf = dh->fc[0].freq;
    int halfband = m_maxband / 2;
    int min = MAX(muf - halfband, MIN_CHN_FREQ);
    int max = MIN(min + m_maxband, MAX_CHN_FREQ);

    /* === step1.估计MUF频点的性能 === */
    int mufSnr = dh->fc[0].snr;
    int mufMufday = dh->fc[0].mufday;
    int mufRnd = m_mufRnd.rab(glbChId, 0, 100);
    if (mufRnd < mufMufday) {
        int u = GRN_U(mufSnr);
        int g = GRN_G(mufSnr);
        int expSnr = m_mufRnd.grn(glbChId, u, g);
        out.mufVld = (expSnr > MIN_SNR);
        out.mufSnr = expSnr;
    }

    /* === step2.估计当前频点的性能 === */
    /* 是否在可通频带 */
    int fc = GLB2FREQ(glbChId);
    if ((fc < min) || (fc > max)) {
        return ENV_INV_GLB;
    }

    /* 找MUFday/SNR */
    int snr = mufSnr;
    int mufday = mufMufday;
    for (int i = 1; i < MAX_FREQ_NUM; i++) {
        if (dh->fc[i].freq >= fc) {
            mufday = dh->fc[i].mufday;
            snr = dh->fc[i].snr;
            break;
        }
    }

    /* 可用概率太低 */
    if (mufday < 5) {
        return ENV_PROB_LO;
    }

    /* 随机数拟合MUFday/SNR */
    int rnd = m_frqRnd.rab(glbChId, 0, 100);
    if (rnd < mufday) {
        int u = GRN_U(snr);
        int g = GRN_G(snr);
        int expSnr = m_frqRnd.grn(glbChId, u, g);
        out.isValid = (expSnr > MIN_SNR);
        out.snr = expSnr;
    }

    return ENV_OK;
}

// 根据信道号获取底噪
const int* WEnv::noise(void)
{
    static int s_noise[NOISE_NUM] = {
    #include "noise.txt"
    };

    return s_noise;
}

