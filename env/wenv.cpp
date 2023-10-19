#include "wenv.h"
#include "sql/sqlite3.h"
#include <QMessageBox>

// 系数
#define FOT_COEF 0.80f
#define MUF_COEF 1.10f
#define LUF_COEF 0.50f

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
        DbFreq* df;
        if ((hour < MAX_HOUR_NUM) && (hour > 0)) {
            df = &m_dbMonth.hr[hour].fc[fid];
        } else { /* 24h填到零晨 */
            df = &m_dbMonth.hr[0].fc[fid];
        }

        df->mufday = mufday;
        df->freq = freq;
        df->snr = snr;
        df->rel = rel;
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
    out.fotVld = false;
    out.snr = INV_SNR;
    out.fotSnr = INV_SNR;
    out.n0 = MIN_PN0;

    /* 时间检查 */
    int ret = check(in);
    if (ret != ENV_OK) {
        return ret;
    }

    /* FOT评估 */
    fot(in, out);

    /* 计算可用标志 */
    return est(in, out);
}

// 根据时戳和信道号结合Model计算可用标志和SNR估计值
int WEnv::est(const EnvIn& in, EnvOut& out)
{
    /* 获取底噪 */
    int glbChId = in.glbChId;
    out.n0 = noise(glbChId);

    /* 获取Hour信息 */
    const DbHour* dh = &m_dbMonth.hr[in.hour];

    /* step1.计算可通频段 */
    int muf = dh->fc[0].freq;
    int max = upper(muf);
    int min = lower(muf);

    /* 是否在可通频带 */
    int fc = GLB2FREQ(glbChId);
    if ((fc < min) || (fc > max)) {
        return ENV_INV_GLB;
    }

    /* step2.找MUFday/SNR */
    int snr, mufday;
    index(dh, fc, mufday, snr);

    /* 可用概率太低 */
    if (mufday < 10) {
        return ENV_PROB_LO;
    }

    /* step3.随机数拟合MUFday/SNR */
    int rnd = m_frqRnd.rab(glbChId, 0, 99);
    if (rnd < mufday) {
        double u = GRN_U(snr);
        double g = GRN_G(snr);
        int expSnr = m_frqRnd.grn(glbChId, u, g);
        out.isValid = (expSnr > MIN_SNR);
        out.snr = expSnr;
    }

    return ENV_OK;
}

// 根据时戳和信道号结合Model计算FOT频点性能
void WEnv::fot(const EnvIn& in, EnvOut& out)
{
    /* 计算FOT */
    const DbHour* dh = &m_dbMonth.hr[in.hour];
    int fx = (int)(dh->fc[0].freq * FOT_COEF);

    /* 找MUFday/SNR */
    int snr, mufday;
    index(dh, fx, mufday, snr);

    /* 随机数拟合MUFday/SNR */
    int glbChId = in.glbChId;
    int rnd = m_fotRnd.rab(glbChId, 0, 100);
    if (rnd < mufday) {
        double u = GRN_U(snr);
        double g = GRN_G(snr);
        int expSnr = m_fotRnd.grn(glbChId, u, g);
        out.fotVld = (expSnr > MIN_SNR);
        out.fotSnr = expSnr;
    }
}

// 查找fc对应的区间，计算MUFday和SNR
bool WEnv::index(const DbHour* dh, int fc, int& mufday, int& snr)
{
    int i, j, k;

    j = MAX_FREQ_NUM - 1;
    if (fc <= dh->fc[1].freq) { /* 低于最小 */
        mufday = dh->fc[1].mufday;
        snr = dh->fc[1].snr;
        return false;
    } else if (fc >= dh->fc[j].freq) { /* 大于最大 */
        mufday = dh->fc[j].mufday;
        snr = dh->fc[j].snr;
        return false;
    } else { /* 在中间 */
        /* 从后往前找 */
        for (i = k = j; i > 0; i--) {
            if (fc >= dh->fc[i].freq) {
                k = i;
                break;
            }
        }

        /* 先计算占比 */
        float fi = dh->fc[k].freq;
        float fj = dh->fc[k + 1].freq;
        float coef = (fc - fi) / (fj - fi);

        /* 在i和i+1之间插值 */
        float si = dh->fc[k].snr;
        float sj = dh->fc[k + 1].snr;
        float mi = dh->fc[k].mufday;
        float mj = dh->fc[k + 1].mufday;
        snr = (int)(si + (sj - si) * coef);
        mufday = (int)(mi + (mj - mi) * coef);
        return true;
    }
}

// 获取MUF值KHz
int WEnv::muf(int hour) const
{
    return m_dbMonth.hr[hour].fc[0].freq;
}

// 频率上限KHz
int WEnv::upper(int muf)
{
    return (int)MIN(muf * MUF_COEF, MAX_CHN_FREQ);
}

// 频率下限KHz
int WEnv::lower(int muf)
{
    int minFc = muf * LUF_COEF;
    int maxFc = MIN(muf * MUF_COEF, MAX_CHN_FREQ);
    return (int)MAX(MAX(minFc, maxFc - m_maxband), MIN_CHN_FREQ);
}

// 根据信道号获取底噪
int WEnv::noise(int glbChId)
{
    const int* pnoise = noise();
    int locChId = glbChId / CHN_SCAN_STEP;
    int subId = locChId / 3; /* 3个信道压缩 */
    return pnoise[subId];
}

// 获取所有底噪数据
const int* WEnv::noise(void)
{
    static int s_noise[NOISE_NUM] = {
    #include "noise.txt"
    };

    return s_noise;
}

