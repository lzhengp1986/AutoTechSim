#include "mainwin.h"
#include "ui_mainwin.h"
#include "env/model.h"
#include "env/dbcreator.h"

MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWin)
{
    ui->setupUi(this);
    setup_win();
    setup_db();
}

MainWin::~MainWin()
{
    free_win();
    delete ui;
}

// 设置win
void MainWin::setup_win(void)
{
    /* 添加label */
    m_label = new WLabel;
    for (int i = 0; i < WLabel::LABLE_NUM; i++) {
        ui->statusbar->addWidget(m_label->get_label(i));
    }

    /* 添加chart */
    m_chart = new WChart;
    ui->chartView->setChart(m_chart->get_chart());
    ui->chartView->setAttribute(Qt::WA_TranslucentBackground);
    ui->chartView->setStyleSheet("background:transparent");
}

// 释放win
void MainWin::free_win(void)
{
    delete m_label;
    delete m_chart;
}

// 数据库列表
void MainWin::setup_db(void)
{
    m_dbList << "Chengdu-Muchuan" << "Chengdu-Xian" << "Chengdu-Saya";
}

void MainWin::on_actTime_triggered(void)
{

}

void MainWin::on_actModel_triggered(void)
{
    Model* model = new Model;
    model->set_list(m_dbList);
    int ret = model->exec();
    if (ret == QDialog::Accepted) {
        /* 获取索引/月份 */
        int index = model->get_index();
        int month = model->get_month();

        /* 转换成文件名 */
        QString fn = m_dbList.at(index);
        QString m = QString("-%1").arg(month, 2, 10, QLatin1Char('0'));

        /* 更新背景图片 */
        this->setObjectName("MainWin");
        QString pic = ":/png/" + fn + m + ".png";
        this->setStyleSheet("#MainWin{border-image:url(" + pic + ")}");

        /* 更新数据库 */
        QString db = "./png/" + fn + ".db";
    }
}

void MainWin::on_actRequest_triggered(void)
{

}

void MainWin::on_actStrategy_triggered(void)
{

}

void MainWin::on_actDatabase_triggered(void)
{
    QDialog* db = new dbCreator;
    int ret = db->exec();
    if (ret == QDialog::Accepted) {
        /* nothing to do */
    }
}

void MainWin::on_actContent_triggered(void)
{

}

void MainWin::on_actCopyright_triggered(void)
{

}
