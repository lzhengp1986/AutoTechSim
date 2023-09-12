#include "mainwin.h"
#include "ui_mainwin.h"
#include "env/dbdlg.h"
#include <QMessageBox>
#include <QDir>

MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWin)
{
    ui->setupUi(this);
    setup_win();
    setup_time();
    setup_model();
    setup_sim();

    /* 更新参数 */
    update_time(m_model);
    update_model(m_model);
}

MainWin::~MainWin()
{
    free_sim();
    free_model();
    free_time();
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
    m_label = nullptr;
    m_chart = nullptr;
}

// 设置Model
void MainWin::setup_model(void)
{
    m_model = new ModelCfg;
    m_model->month = 1;
    m_model->year = 2023;
    m_model->dbDesc
            << "成都市区-乐山沐川"
            << "成都市区-陕西西安"
            << "成都市区-海南三亚";

    m_model->dbIndex = 0;
    m_model->bandIndex = 0;
}

// 更新Model
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
    int rc = m_sim->m_env->setup(month, dbFile);
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
    delete m_model;
    m_model = nullptr;
}

// 设置time
void MainWin::setup_time(void)
{
    m_time = new Time;
    m_time->year = 2023;
    m_time->month = 1;
    m_time->day = 1;
    m_time->hour = 1;
    m_time->min = 0;
    m_time->sec = 0;

    /* 定时器子线程 */
    m_tmr = new QTimer;
    m_thread = new QThread(this);
    connect(m_tmr, SIGNAL(timeout()), this, SLOT(on_sim_timer_timeout()));
    m_tmr->start(1000);
    m_tmr->moveToThread(m_thread);
    m_thread->start();
}

// 更新Time += sec
void MainWin::update_time(int sec)
{
    m_time->sec += sec;
    if (m_time->sec < 60) {
        goto UPDATE_LABEL;
    }

    m_time->sec -= 60;
    m_time->min++;
    if (m_time->min < 60) {
        goto UPDATE_LABEL;
    }

    m_time->min -= 60;
    m_time->hour++;
    if (m_time->hour < 24) {
        goto UPDATE_LABEL;
    }

    m_time->hour -= 23;
    m_time->day++;

UPDATE_LABEL:
    m_label->set_time(m_time);
}

// 更新Time
void MainWin::update_time(const ModelCfg* cfg)
{
    m_time->year = cfg->year;
    m_time->month = cfg->month;
    m_time->day = 1;
    m_time->hour = 1;
    m_time->min = 0;
    m_time->sec = 0;
    m_label->set_time(m_time);
}

// 释放time
void MainWin::free_time(void)
{
    m_tmr->stop();
    m_thread->quit();
    delete m_tmr;
    delete m_thread;
    delete m_time;
    m_tmr = nullptr;
    m_thread = nullptr;
    m_time = nullptr;
}

void MainWin::setup_sim(void)
{
    m_sim = new LinkSim;
}

void MainWin::free_sim(void)
{
    delete m_sim;
    m_sim = nullptr;
}

// 定时器超时处理
void MainWin::on_sim_timer_timeout(void)
{
    /* 更新时间 */
    int speedIndex = m_sim->m_link->tmrSpeedIndex;
    int speed = LinkDlg::timerSpeed(speedIndex);
    update_time(speed);

    /* 建链仿真 */
    int state = m_sim->simulate(m_time);
    m_label->set_state(state);
}

void MainWin::on_actModel_triggered(void)
{
    ModelDlg* dlg = new ModelDlg(this);
    dlg->para2dlg(m_model);

    int ret = dlg->exec();
    if (ret == QDialog::Accepted) {
        /* 获取模型参数 */
        ModelCfg cfg = *m_model;
        dlg->dlg2para(&cfg);

        /* 更新数据库 */
        int flag = update_model(&cfg);
        if (flag != 0) {
            return;
        }

        /* 更新模型参数 */
        *m_model = cfg;

        /* 更新时间参数 */
        update_time(m_model);
    }

    delete dlg;
}

void MainWin::on_actRequest_triggered(void)
{
    LinkDlg* dlg = new LinkDlg(this);
    dlg->para2dlg(m_sim->m_link);
    int ret = dlg->exec();
    if (ret == QDialog::Accepted) {
        dlg->dlg2para(m_sim->m_link);
    }
}

void MainWin::on_actStrategy_triggered(void)
{
    AutoDlg* dlg = new AutoDlg(this);
    dlg->para2dlg(m_sim->m_auto);
    int ret = dlg->exec();
    if (ret == QDialog::Accepted) {
        dlg->dlg2para(m_sim->m_auto);
    }

    /* 启动仿真 */
    m_label->set_state(IDLE);
    m_sim->start();
}

void MainWin::on_actDatabase_triggered(void)
{
    QDialog* dlg = new DbDlg(this);
    int ret = dlg->exec();
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
