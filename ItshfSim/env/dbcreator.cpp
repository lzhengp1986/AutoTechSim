#include "dbcreator.h"
#include "ui_dbcreator.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

/* 固定参数 */
#define CHAR_OFF 6 /* 符号偏移 */
#define DATA_WID 5 /* 数据宽 */
#define FREQ_NUM 12 /* 频点数 */

dbCreator::dbCreator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dbCreator)
{
    ui->setupUi(this);
    ui->prgBar->setValue(0);
}

dbCreator::~dbCreator()
{
    delete ui;
}

void dbCreator::on_brsBtn_clicked(void)
{
    QString path = QDir::currentPath();
    QString filter = "VOACAP Output(*.out);;所有文件(*.*)";
    QString fn = QFileDialog::getOpenFileName(this, "Open VOACAP file", path, filter);
    ui->input->setText(fn);

    /* 文件不存在 */
    if (fn.isEmpty()) {
        QMessageBox::warning(this, "Warning",
            "voacap.out dose not specified.");
        return;
    }

    /* 保存文件名 */
    m_fileName = fn;
}

void dbCreator::on_extBtn_clicked(void)
{
    /* step0.复位进度条 */
    int rate = 0;
    ui->prgBar->setValue(0);

    /* step1.创建db */
    sqlite3* db = nullptr;
    db_open(&db);

    /* step2.读出内容 */
    int n = read_valid_lines();

    int i = 0;
    bool isHead = false;
    int year, month, hour, ssn;
    QList<int> freq, dbu, snr, rel, sprb;
    int iFreq, iDbu, iSnr, iRel, iSprb;

    /* step3.解析文本 */
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
                hour = trans_one_hour(i, freq, dbu, snr, rel, sprb);
                isHead = (hour < 24);
                i += HOUR_LINES;

                /* 保存到db */
                for (int j = 0; j < FREQ_NUM; j++) {
                    iFreq = freq.at(j);
                    iDbu = dbu.at(j);
                    iSnr = snr.at(j);
                    iRel = rel.at(j);
                    iSprb = sprb.at(j);
                    db_insert(db, year, month, ssn, iFreq, iDbu, iSnr, iRel, iSprb);
                }
            } else {
                i++;
            }
        }

        /* 进度显示 */
        rate = 100 * (i + 1) / n;
        ui->prgBar->setValue(rate);
    }

    /* step4.关闭db */
    db_close(db);
}

int dbCreator::read_valid_lines(void)
{
    m_content.clear();
    QFile file(m_fileName);

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
bool dbCreator::is_valid_hour(int i)
{
    bool isFrq = m_content.at(i).endsWith("FREQ");
    bool isDbu = m_content.at(i + 1).endsWith("DBU");
    bool isSnr = m_content.at(i + 2).endsWith("SNR");
    bool isRel = m_content.at(i + 3).endsWith("REL");
    bool isPrb = m_content.at(i + 4).endsWith("S PRB");
    return (isFrq && isDbu && isSnr && isRel && isPrb);
}

/* 转换从i行开始的hour结构 */
int dbCreator::trans_one_hour(int i, QList<int>& freq, QList<int>& dbu, QList<int>& snr, QList<int>& rel, QList<int>& sprb)
{
    int hour = 0;
    trans_freq(i, hour, freq);
    trans_data(i + 1, dbu);
    trans_data(i + 2, snr);
    trans_data(i + 3, rel);
    trans_data(i + 4, sprb);
    return hour;
}

/* 转换第i行的head结构 */
void dbCreator::trans_head(int i, int& year, int& month, int& ssn)
{
    QString line = m_content.at(i);
    year = line.mid(9, 4).trimmed().toInt();
    float s = line.mid(28, 6).trimmed().toFloat();
    ssn = static_cast<int>(s);

    /* 月份转换 */
    QString m = line.mid(2, 4).trimmed();
    if (m == "Jan") {
        month = 0;
    } else if (m == "Feb") {
        month = 1;
    } else if (m == "Mar") {
        month = 2;
    } else if (m == "Apr") {
        month = 3;
    } else if (m == "May") {
        month = 4;
    } else if (m == "Jun") {
        month = 5;
    } else if (m == "Jul") {
        month = 6;
    } else if (m == "Aug") {
        month = 7;
    } else if (m == "Sep") {
        month = 8;
    } else if (m == "Oct") {
        month = 9;
    } else if (m == "Nov") {
        month = 10;
    } else {
        month = 11;
    }
}

/* 转换第i行的data结构 */
void dbCreator::trans_data(int i, QList<int>& list)
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
void dbCreator::trans_freq(int i, int& hour, QList<int>& list)
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
void dbCreator::split(const QString& line, QList<int>& list)
{
    int i, j, k;

    list.clear();
    for (i = 0; i < FREQ_NUM; i++) {
        j = CHAR_OFF + i * DATA_WID;
        QString text = line.mid(j, DATA_WID).trimmed();
        if (text.contains('E')) {
            k = text.left(1).toInt();
        } else if (text.contains("F1")) {
            k = text.left(1).toInt() * 10;
        } else if (text.contains("F2")) {
            k = text.left(1).toInt() * 20;
        } else {
            k = text.toInt();
        }
        list.append(k);
    }
}

/* 字符串拆分float */
void dbCreator::split(const QString& line, QList<float>& list)
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

void dbCreator::db_open(sqlite3** db)
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
    }

    /* 删除内容 */
    char* errMsg = nullptr;
    const char *sql1 = "DROP TABLE IF EXISTS ITU";
    int rc1 = sqlite3_exec(*db, sql1, 0, 0, &errMsg);
    if (rc1 != SQLITE_OK) {
        QMessageBox::warning(this, "Warning", errMsg);
        sqlite3_free(errMsg);
    }

    /* 创建表格 */
    const char *sql2 = "CREATE TABLE  IF NOT EXISTS ITU(year INTEGER, month INTEGER, ssn INTEGER,"
                       "freq INTEGER, dbu INTEGER, snr INTEGER, rel INTEGER, sprb INTEGER,"
                       "UNIQUE(year, month, ssn, freq) ON CONFLICT IGNORE)";
    int rc2 = sqlite3_exec(*db, sql2, 0, 0, &errMsg);
    if (rc2 != SQLITE_OK) {
        QMessageBox::warning(this, "Warning", errMsg);
        sqlite3_free(errMsg);
    }
}

void dbCreator::db_insert(sqlite3* db, int year, int month, int ssn, int freq, int dbu, int snr, int rel, int sprb)
{
    char* errMsg = nullptr;
    char* sql = sqlite3_mprintf("INSERT INTO ITU VALUES(%d,%d,%d,%d, %d,%d,%d,%d)", year, month, ssn, freq, dbu, snr, rel, sprb);
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        QMessageBox::warning(this, "Warning", errMsg);
        sqlite3_free(errMsg);
    }
    sqlite3_free(sql);
}

void dbCreator::db_close(sqlite3* db)
{
    sqlite3_close(db);
}
