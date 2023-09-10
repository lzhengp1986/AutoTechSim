#ifndef LINKWIN_H
#define LINKWIN_H

#include <QDialog>

namespace Ui {
class LinkWin;
}

typedef struct {
    int speed; /* 仿真定时速度 */
    int reqPtn; /* 建链请求类型：周期或随机 */
} LinkCfgPara;

class LinkWin : public QDialog
{
    Q_OBJECT

public:
    explicit LinkWin(QWidget *parent = nullptr);
    ~LinkWin();

    /* api */

private:
    Ui::LinkWin *ui;
};

#endif // LINKWIN_H
