#ifndef LINKDLG_H
#define LINKDLG_H

#include <QDialog>

namespace Ui {
class LinkDlg;
}

typedef struct {
    int speed; /* 仿真定时速度 */
    int reqPtn; /* 建链请求类型：周期或随机 */
} LinkCfgPara;

class LinkDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LinkDlg(QWidget *parent = nullptr);
    ~LinkDlg();

private:
    Ui::LinkDlg *ui;
};

#endif // LINKDLG_H
