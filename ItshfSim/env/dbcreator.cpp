#include "dbcreator.h"
#include "ui_dbcreator.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

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
    QString fn = QFileDialog::getOpenFileName(this, "Open voacap.out", path, filter);
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

    /* step2.读出内容 */
    int n = read_valid_lines();

    int i = 0;
    bool isHead = false;
    int year, month, hour, ssn;
    QList<int> freq, dbu, snr, rel, sprb;

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
            } else {
                i++;
            }
        }

        /* 进度显示 */
        rate = 100 * (i + 1) / n;
        ui->prgBar->setValue(rate);
    }

    /* step4.关闭db */
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
}

/* 转换第i行的data结构 */
void dbCreator::trans_data(int i, QList<int>& list)
{
    list.clear();
}

/* 转换第i行的freq结构 */
void dbCreator::trans_freq(int i, int& hour, QList<int>& list)
{
    list.clear();
}

