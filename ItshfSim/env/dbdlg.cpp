#include "dbdlg.h"
#include "ui_dbdlg.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

/* 固定参数 */
#define CHAR_OFF 6 /* 符号偏移 */
#define DATA_WID 5 /* 数据宽 */
#define FREQ_NUM 12 /* 频点数 */

DbDlg::DbDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbDlg)
{
    ui->setupUi(this);
    ui->prgBar->setValue(0);
}

DbDlg::~DbDlg()
{
    delete ui;
}

void DbDlg::on_brsBtn_clicked(void)
{
    QString path = QDir::currentPath();
    QString filter = "VOACAP Output(*.out);;所有文件(*.*)";
    QString fn = QFileDialog::getOpenFileName(this, "Open VOACAP file", path, filter);
    ui->input->setText(fn);

    /* 保存文件名 */
    m_fileName = fn;
}

void DbDlg::on_extBtn_clicked(void)
{
    /* step0.文件不存在 */
    if (m_fileName.isEmpty()) {
        QMessageBox::warning(this, "Warning",
            "voacap.out dose not specified.");
        return;
    }

    /* step1.读出内容 */
    int n = read_valid_lines(m_fileName);

    /* step2.复位进度条 */
    int rate = 0;
    ui->prgBar->setValue(0);

    /* step3.创建db */
    sqlite3* db = nullptr;
    bool flag = db_open(&db);
    if (flag == false) {
        db_close(db);
        return;
    }

    int i = 0;
    bool isHead = false;
    int year, month, hour, ssn;
    QList<int> freq, mufday, dbu, snr, rel, sprb;
    int iFreq, iMufday, iDbu, iSnr, iRel, iSprb;

    /* step4.解析文本 */
    while (i < n) {
        /* 拼接page */
        if (isHead == false) {
            QString line = m_content.at(i);
            isHead = line.endsWith("degrees");
            if (isHead == true) {
                trans_head(i, year, month, ssn);
            }

            i++;
        } else {
            if (is_valid_hour(i)) {
                /* 数据转换 */
                hour = trans_one_hour(i, freq, mufday, dbu, snr, rel, sprb);
                isHead = (hour < 24);
                i += HOUR_LINES;

                /* 保存到db */
                for (int j = 0; j < FREQ_NUM; j++) {
                    iFreq = freq.at(j);
                    iMufday = mufday.at(j);
                    iDbu = dbu.at(j);
                    iSnr = snr.at(j);
                    iRel = rel.at(j);
                    iSprb = sprb.at(j);
                    db_insert(db, year, month, ssn, hour, iFreq, iMufday, iDbu, iSnr, iRel, iSprb);
                }
            } else {
                i++;
            }
        }

        /* 进度显示 */
        rate = 100 * (i + 1) / n;
        ui->prgBar->setValue(rate);
    }

    /* step5.关闭db */
    db_close(db);
}

int DbDlg::read_valid_lines(const QString& fn)
{
    m_content.clear();
    QFile file(fn);

    /* 判断文件是否存在 */
    if (! file.exists()) {
        QMessageBox::warning(this, "Warning",
            "voacap.out dose not exist.");
        return 0;
    }

    /* 打开文件 */
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Warning",
            "Fail to open voacap.out.");
        return 0;
    }

    /* 读出所有内容 */
    while (! file.atEnd()) {
        QByteArray line = file.readLine();
        QString text = line.left(line.size() - 1);
        if (text.endsWith("degrees")
            || text.endsWith("FREQ")
            || text.endsWith("MUFday")
            || text.endsWith("DBU")
            || text.endsWith("SNR")
            || text.endsWith("REL")
            || text.endsWith("S PRB")) {
            m_content << text;
        }
    }

    file.close();
    return m_content.size();
}

/* 判断content中从id行开始的n行是否为有效的hour结构 */
bool DbDlg::is_valid_hour(int i)
{
    bool isFrq = m_content.at(i).endsWith("FREQ");
    bool isDay = m_content.at(i + 1).endsWith("MUFday");
    bool isDbu = m_content.at(i + 2).endsWith("DBU");
    bool isSnr = m_content.at(i + 3).endsWith("SNR");
    bool isRel = m_content.at(i + 4).endsWith("REL");
    bool isPrb = m_content.at(i + 5).endsWith("S PRB");
    return (isFrq && isDay && isDbu && isSnr && isRel && isPrb);
}

/* 转换从i行开始的hour结构 */
int DbDlg::trans_one_hour(int i, QList<int>& freq, QList<int>& mufDay, QList<int>& dbu, QList<int>& snr, QList<int>& rel, QList<int>& sprb)
{
    int hour = 0;
    trans_freq(i, hour, freq);
    trans_data(i + 1, mufDay);
    trans_data(i + 2, dbu);
    trans_data(i + 3, snr);
    trans_data(i + 4, rel);
    trans_data(i + 5, sprb);
    return hour;
}

/* 转换第i行的head结构 */
void DbDlg::trans_head(int i, int& year, int& month, int& ssn)
{
    /* year */
    QString line = m_content.at(i);
    year = line.mid(9, 4).trimmed().toInt();

    /* ssn */
    float s = line.mid(28, 6).trimmed().toFloat();
    ssn = static_cast<int>(s);

    /* month */
    QString m = line.mid(2, 4).trimmed();
    month = trans_month(m);
}

int DbDlg::trans_month(const QString& month)
{
    int m = 0;
    if (month == "Jan") {
        m = 1;
    } else if (month == "Feb") {
        m = 2;
    } else if (month == "Mar") {
        m = 3;
    } else if (month == "Apr") {
        m = 4;
    } else if (month == "May") {
        m = 5;
    } else if (month == "Jun") {
        m = 6;
    } else if (month == "Jul") {
        m = 7;
    } else if (month == "Aug") {
        m = 8;
    } else if (month == "Sep") {
        m = 9;
    } else if (month == "Oct") {
        m = 10;
    } else if (month == "Nov") {
        m = 11;
    } else {
        m = 12;
    }

    return m;
}

/* 转换第i行的data结构 */
void DbDlg::trans_data(int i, QList<int>& list)
{
    list.clear();
    QString line = m_content.at(i);
    if (line.endsWith("DBU") || line.endsWith("SNR")) {
        split(line, list);
    } else {
        QList<float> flist;
        split(line, flist);
        for (int i = 0; i < flist.size(); i++) {
            float v = flist.at(i) * 100 + 0.5f;
            int j = static_cast<int>(v);
            list.append(j);
        }
    }
}

/* 转换第i行的freq结构 */
void DbDlg::trans_freq(int i, int& hour, QList<int>& list)
{
    QString line = m_content.at(i);

    /* 转换hour */
    QString hr = line.mid(2, 4).trimmed();
    hour = static_cast<int>(hr.toFloat());

    /* 转换频点 */
    QList<float> flist;
    split(line, flist);

    /* 数据转换 */
    list.clear();
    for (int i = 0; i < flist.size(); i++) {
        float v = flist.at(i) * 1000 + 0.5f;
        int j = static_cast<int>(v);
        list.append(j);
    }
}

/* 字符串拆分int */
void DbDlg::split(const QString& line, QList<int>& list)
{
    int i, j;

    list.clear();
    for (i = 0; i < FREQ_NUM; i++) {
        j = CHAR_OFF + i * DATA_WID;
        QString text = line.mid(j, DATA_WID).trimmed();
        int k = text.toInt();
        list.append(k);
    }
}

/* 字符串拆分float */
void DbDlg::split(const QString& line, QList<float>& list)
{
    int i, j;
    list.clear();
    for (i = 0; i < FREQ_NUM; i++) {
        j = CHAR_OFF + i * DATA_WID;
        QString text = line.mid(j, DATA_WID).trimmed();
        float k = text.toFloat();
        list.append(k);
    }
}

bool DbDlg::db_open(sqlite3** db)
{
    /* 截取db路径 */
    int i = m_fileName.size() - 1;
    while (i > 0) {
        if (m_fileName.at(i) == '.') {
            break;
        } else {
            i--;
        }
    }

    QString fn = m_fileName.left(i + 1) + "db";
    const char* file = fn.toStdString().c_str();

    /* 打开db */
    int ret = sqlite3_open_v2(file, db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (ret) {
        QMessageBox::warning(this, "Warning", "Can not open database file.");
        return false;
    }

    /* 删除表格 */
    char* errMsg = nullptr;
    const char *sql1 = "DROP TABLE IF EXISTS ITU";
    int rc1 = sqlite3_exec(*db, sql1, 0, 0, &errMsg);
    if (rc1 != SQLITE_OK) {
        QString info = QString(QLatin1String(errMsg));
        QMessageBox::warning(this, "Warning", info);
        sqlite3_free(errMsg);
        return false;
    }

    /* 创建表格 */
    const char *sql2 = "CREATE TABLE  IF NOT EXISTS ITU(year INTEGER, month INTEGER, ssn INTEGER, hour INTEGER,"
                       "freq INTEGER, mufday INTEGER, dbu INTEGER, snr INTEGER, rel INTEGER, sprb INTEGER,"
                       "UNIQUE(year, month, ssn, hour, freq) ON CONFLICT IGNORE)";
    int rc2 = sqlite3_exec(*db, sql2, 0, 0, &errMsg);
    if (rc2 != SQLITE_OK) {
        QString info = QString(QLatin1String(errMsg));
        QMessageBox::warning(this, "Warning", info);
        sqlite3_free(errMsg);
    }

    /* transaction */
    sqlite3_exec(*db, "BEGIN TRANSACTION", 0, 0, NULL);
    return true;
}

void DbDlg::db_insert(sqlite3* db, int year, int month, int ssn, int hour,
                      int freq, int mufday, int dbu, int snr, int rel, int sprb)
{
    char* errMsg = nullptr;
    char* sql = sqlite3_mprintf("INSERT INTO ITU VALUES(%d,%d,%d,%d,%d, %d,%d,%d,%d,%d)",
                                year, month, ssn, hour, freq, mufday, dbu, snr, rel, sprb);
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        QString info = QString(QLatin1String(errMsg));
        QMessageBox::warning(this, "Warning", info);
        sqlite3_free(errMsg);
    }
    sqlite3_free(sql);
}

void DbDlg::db_close(sqlite3* db)
{
    sqlite3_exec(db, "END TRANSACTION", 0, 0, NULL);
    sqlite3_close(db);
}
