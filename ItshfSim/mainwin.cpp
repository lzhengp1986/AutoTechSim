#include "mainwin.h"
#include "ui_mainwin.h"
#include "env/dbcreator.h"
#include <QMessageBox>
#include <QDir>

MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWin)
{
    ui->setupUi(this);
    setup_win();
    setup_env();
    setup_model();
}

MainWin::~MainWin()
{
    free_env();
    free_win();
    free_model();
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

// 设置env
void MainWin::setup_env(void)
{
    m_env = new WEnv;
}

// 释放env
void MainWin::free_env(void)
{
    delete m_env;
}

// 设置Model
void MainWin::setup_model(void)
{
    m_cfg = new ModelCfg;
    m_cfg->month = 1;
    m_cfg->year = 2023;
    m_cfg->dbDesc
            << "成都市区-乐山沐川"
            << "成都市区-陕西西安"
            << "成都市区-海南三亚";

    m_cfg->dbIndex = 0;
    m_cfg->bandIndex = 0;
    update_model(m_cfg);
}

// 设置Model
int MainWin::update_model(const ModelCfg* cfg)
{
    /* 获取索引/月份 */
    int year = cfg->year;
    int month = cfg->month;
    int dbIndex = cfg->dbIndex;

    /* 转换成文件名 */
    QString pre = QString("%1").arg(dbIndex, 2, 10, QLatin1Char('0'));
    QString pos = QString("%1").arg(month, 2, 10, QLatin1Char('0'));
    QString prefix = "./png/" + QString::number(year) + "/" + pre;

    /* 更新数据库 */
    QString dbFile = prefix + "/voacapx.db";
    int rc = m_env->setup(month, dbFile);
    if (rc != 0) {
        QMessageBox::warning(this, "Warning", "Fail to setup model!");
        return rc;
    }

    /* 更新背景图片 */
    this->setObjectName("MainWin");
    QString pic = prefix + '/' + pos + ".png";
    this->setStyleSheet("#MainWin{border-image:url(" + pic + ")}");
    return rc;
}

// 释放model
void MainWin::free_model(void)
{
    delete m_cfg;
}

void MainWin::on_actModel_triggered(void)
{
    Model* model = new Model(this);
    model->para2win(m_cfg);

    int ret = model->exec();
    if (ret == QDialog::Accepted) {
        /* 获取模型参数 */
        ModelCfg cfg = *m_cfg;
        model->win2para(&cfg);

        /* 更新数据库 */
        int flag = update_model(&cfg);
        if (flag != 0) {
            return;
        }

        /* 更新模型参数 */
        *m_cfg = cfg;
    }

    delete model;
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
