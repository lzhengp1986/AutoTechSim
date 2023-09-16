#ifndef MAINWIN_H
#define MAINWIN_H

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
    void on_new_time(const Time* ts);
    void on_new_state(int state, int dsec);
    void on_new_chan(int glbChId, int snr, int n0, int regret);
    void on_new_sts(int scanTry, int scanFrq, int linkNum, int testNum);

    void on_actModel_triggered(void);
    void on_actStrategy_triggered(void);
    void on_actDatabase_triggered(void);
    void on_actContent_triggered(void);
    void on_actCopyright_triggered(void);

    void on_scanBlack_clicked(void);
    void on_scanBlue_clicked(void);
    void on_scanCyan_clicked(void);
    void on_scanMagenta_clicked(void);
    void on_linkBlack_clicked(void);
    void on_linkBlue_clicked(void);
    void on_linkCyan_clicked(void);
    void on_linkMagenta_clicked(void);

private:
    void setup_win(void);
    void free_win(void);

    void setup_model(void);
    int update_model(const ModelCfg* cfg);
    void free_model(void);

    void setup_sim(void);
    void free_sim(void);

private:
    /* 主窗口 */
    Ui::MainWin *ui;
    WLabel *m_label;
    WChart *m_chart;

    /* ITS模型 */
    ModelCfg *m_model;

    /* 仿真状态机 */
    LinkSim *m_sim;
    Time *m_time;
};
#endif // MAINWIN_H
