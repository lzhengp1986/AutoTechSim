#ifndef MAINWIN_H
#define MAINWIN_H

#include "env/wenv.h"
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

private:
    /* 主窗口 */
    Ui::MainWin *ui;
    WLabel *m_label;
    WChart *m_chart;

    /* ITS模型 */
    WEnv *m_env;
};
#endif // MAINWIN_H
