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
    int i = 0;
    int rate = 0;
    ui->prgBar->setValue(0);

    /* 解析文本 */
    int n = read_valid_lines();
    while (i < n) {
        if (is_valid_hour(i)) {
            trans_one_hour(i);
            i += HOUR_LINES;
        } else {
            i++;
        }

        /* 进度显示 */
        rate = 100 * (i + 1) / n;
        ui->prgBar->setValue(rate);
    }
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
            m_content << line;
        }
    }

    file.close();
    return m_content.size();
}

/* 判断content中从id行开始的n行是否为有效的hour结构 */
bool dbCreator::is_valid_hour(int id)
{
    return false;
}

/* 转换从id行开始的hour结构 */
void dbCreator::trans_one_hour(int id)
{

}
