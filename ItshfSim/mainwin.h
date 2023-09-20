#ifndef MAINWIN_H
#define MAINWIN_H

#include "colorpal.h"
#include "env/modeldlg.h"
#include "link/linksim.h"
#include "win/wlabel.h"
#include "win/wchart.h"
#include <QMainWindow>

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
    void on_new_day(void);
    void on_new_time(const Time* ts);
    void on_new_state(int state, int dsec);
    void on_new_chan(int type, int glbChId, int snr, int n0, int regret);
    void on_new_sts(float avgScan, int scanFrq, int linkNum, int testNum);

    void on_scan_color(Qt::GlobalColor color);
    void on_link_color(Qt::GlobalColor color);
    void on_reg_color(Qt::GlobalColor color);
    void on_fnum_color(Qt::GlobalColor color);

    void on_actModel_triggered(void);
    void on_actStrategy_triggered(void);
    void on_actDatabase_triggered(void);
    void on_actContent_triggered(void);
    void on_actCopyright_triggered(void);

private:
    void setup_win(void);
    void free_win(void);
    void setup_pal(void);
    void free_pal(void);

    void setup_model(void);
    int update_model(const ModelCfg* cfg);
    void free_model(void);

    void setup_sim(void);
    void free_sim(void);
    void display(const QString& algName, const QString& sqlRule);

private:
    /* 主窗口 */
    Ui::MainWin *ui;
    WLabel *m_label;
    WChart *m_chart;
    ColorPal *m_pal;

    /* ITS模型 */
    ModelCfg *m_model;

    /* 仿真状态机 */
    LinkSim *m_sim;
    Time *m_time;
};
#endif // MAINWIN_H
