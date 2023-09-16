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
    setup_model();
    setup_win();
    setup_sim();
    setup_pal();

    /* 更新参数 */
    int year = m_model->year;
    int month = m_model->month;
    m_sim->set_time(year, month);
    update_model(m_model);
}

MainWin::~MainWin()
{
    free_pal();
    free_sim();
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
    m_label = nullptr;
    m_chart = nullptr;
}

// 设置Model
void MainWin::setup_model(void)
{
    m_model = new ModelCfg;
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
    int maxband = ModelCfg::get_maxband(cfg->bandIndex);
    int rc = m_sim->m_env->setup(month, maxband, dbFile);
    if (rc != 0) {
        QMessageBox::warning(this, "Warning", "Fail to setup model!");
        return rc;
    }

    /* 更新底噪数据 */
    if (cfg->withNoise == true) {
        const int* pnoise = WEnv::noise();
        m_chart->plot(pnoise, NOISE_NUM);
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

void MainWin::setup_sim(void)
{
    /* 构造sim */
    m_sim = new LinkSim(this);
    m_time = new Time;

    /* 信号连接 */
    connect(m_sim, SIGNAL(new_state(int, int)), this, SLOT(on_new_state(int, int)));
    connect(m_sim, SIGNAL(new_time(const Time*)), this, SLOT(on_new_time(const Time*)));
    connect(m_sim, SIGNAL(new_chan(int, int, int, int)), this, SLOT(on_new_chan(int, int, int, int)));
    connect(m_sim, SIGNAL(new_sts(int, int, int, int)), this, SLOT(on_new_sts(int, int, int, int)));

    /* 启动线程 */
    m_sim->start();
}

void MainWin::free_sim(void)
{
    m_sim->stop();
    m_sim->quit();
    delete m_time;
    delete m_sim;
    m_sim = nullptr;
    m_time = nullptr;
}

void MainWin::setup_pal(void)
{
    /* 添加调色板 */
    m_pal = new ColorPal(this);
    ui->colorLayout->addWidget(m_pal);

    /* 信号连接 */
    connect(m_pal, SIGNAL(scan_color(Qt::GlobalColor)), this, SLOT(on_scan_color(Qt::GlobalColor)));
    connect(m_pal, SIGNAL(link_color(Qt::GlobalColor)), this, SLOT(on_link_color(Qt::GlobalColor)));
    connect(m_pal, SIGNAL(reg_color(Qt::GlobalColor)), this, SLOT(on_reg_color(Qt::GlobalColor)));
}

void MainWin::free_pal(void)
{
    delete m_pal;
    m_pal = nullptr;
}

void MainWin::on_new_time(const Time* ts)
{
    *m_time = *ts;
    m_label->set_time(ts);
}

void MainWin::on_new_state(int state, int dsec)
{
    m_label->set_state(state, dsec);
}

void MainWin::on_new_chan(int glbChId, int snr, int n0, int regret)
{
    /* 绘图 */
    float fm = m_time->min / 60.0f;
    float fs = m_time->sec / 3600.0f;
    float hour = m_time->hour + fm + fs;
    float fc = GLB2FREQ(glbChId) / 1000.0f;
    m_chart->plot(hour, fc, snr, regret);

    /* 日志 */
    QString text = QString("%1 %2 %3 %4 %5")
            .arg(hour, 4, 'f', 2)
            .arg(glbChId, 4, 10, QLatin1Char(' '))
            .arg(snr, 3, 10, QLatin1Char(' '))
            .arg(n0, 4, 10, QLatin1Char(' '))
            .arg(regret, 5, 10, QLatin1Char(' '));
    ui->logText->appendPlainText(text);

    /* 状态栏显示 */
    m_label->set_channel(glbChId);
    m_label->set_ratio(snr);
    m_label->set_noise(n0);
}

void MainWin::on_new_sts(int scanTry, int scanFrq, int linkNum, int testNum)
{
    ui->avgScanTry->setText(QString::number(scanTry));
    ui->totalScanNum->setText(QString::number(scanFrq));
    ui->totalLinkNum->setText(QString::number(linkNum));
    ui->totalTestNum->setText(QString::number(testNum));
}

void MainWin::on_scan_color(Qt::GlobalColor color)
{
    m_chart->set_scan_color(color);
}

void MainWin::on_link_color(Qt::GlobalColor color)
{
    m_chart->set_link_color(color);
}

void MainWin::on_reg_color(Qt::GlobalColor color)
{
    m_chart->set_regret_color(color);
}

void MainWin::on_actModel_triggered(void)
{
    m_sim->stop();
    m_chart->clear();
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
        int year = m_model->year;
        int month = m_model->month;
        m_sim->set_time(year, month);
    }

    delete dlg;
}

void MainWin::on_actStrategy_triggered(void)
{
    /* 配置参数 */
    LinkDlg* dlg = new LinkDlg(this);
    dlg->para2dlg(m_sim->m_link);
    int ret = dlg->exec();
    if (ret == QDialog::Accepted) {
        dlg->dlg2para(m_sim->m_link);

        /* 启动仿真 */
        m_sim->trigger();
    }
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
