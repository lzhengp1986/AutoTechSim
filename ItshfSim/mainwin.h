#ifndef MAINWIN_H
#define MAINWIN_H

#include "type.h"
#include "env/wenv.h"
#include "env/modeldlg.h"
#include "link/linkdlg.h"
#include "auto/autodlg.h"
#include "win/wlabel.h"
#include "win/wchart.h"
#include <QMainWindow>
#include <QThread>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWin; }
QT_END_NAMESPACE

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    MainWin(QWidget *parent = nullptr);
    ~MainWin();

private slots:
    void on_sim_timer_timeout(void);
    void on_actModel_triggered(void);
    void on_actRequest_triggered(void);
    void on_actStrategy_triggered(void);
    void on_actDatabase_triggered(void);
    void on_actContent_triggered(void);
    void on_actCopyright_triggered(void);

private:
    void setup_win(void);
    void free_win(void);
    void setup_env(void);
    void free_env(void);

    void setup_model(void);
    int update_model(const ModelCfg* cfg);
    void free_model(void);

    void setup_time(void);
    void update_time(int sec);
    void update_time(const ModelCfg* cfg);
    void free_time(void);

    void setup_link(void);
    void free_link(void);
    void setup_auto(void);
    void free_auto(void);

    void simulate(const Time* ts);
    void sim_idle(const Time* ts);
    void sim_scan(const Time* ts);
    void sim_link(const Time* ts);
    int second(const Time* ts);

private:
    /* 主窗口 */
    Ui::MainWin *ui;
    WLabel *m_label;
    WChart *m_chart;

    /* ITS模型 */
    WEnv *m_env;
    ModelCfg *m_model;

    /* 定时器线程 */
    Time *m_time;
    QTimer *m_tmr;
    QThread *m_thread;

    /* 仿真配置 */
    LinkCfg *m_link;
    AutoCfg *m_auto;
    FreqReq m_req;
    FreqRsp m_rsp;
    Time m_hist;

    /* 状态机 */
    int m_state;
};
#endif // MAINWIN_H
