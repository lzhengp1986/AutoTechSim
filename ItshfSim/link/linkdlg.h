#ifndef LINKDLG_H
#define LINKDLG_H

#include <QDialog>

namespace Ui {
class LinkDlg;
}

typedef struct {
    int tmrSpeedIndex; /* 定时速度 */
    int scanIntvIndex; /* 扫频间隔 */
    int svcIntvIndex; /* 业务间隔 */
    int fcNumIndex; /* 单次请求频点个数 */
} LinkCfg;

class LinkDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LinkDlg(QWidget *parent = nullptr);
    ~LinkDlg();

    /* api */
    void dlg2para(LinkCfg* cfg);
    void para2dlg(const LinkCfg* cfg);

    static int timerSpeed(int index);
    static int scanIntv(int index);
    static int svcIntv(int index);
    static int fcNum(int index);

private:
    Ui::LinkDlg *ui;
};

inline int LinkDlg::timerSpeed(int index)
{
    int speed = 0;
    switch (index) {
    case 0: speed = 1; break;
    case 1: speed = 4; break;
    case 2: speed = 8; break;
    case 3: speed = 16; break;
    case 4: speed = 32; break;
    case 5: speed = 64; break;
    default: speed = 64; break;
    }
    return speed;
}

inline int LinkDlg::scanIntv(int index)
{
    int scanIntv = 0;
    switch (index) {
    case 0: scanIntv = 2; break;
    case 1: scanIntv = 1; break;
    default: scanIntv = 2; break;
    }
    return scanIntv;
}

inline int LinkDlg::svcIntv(int index)
{
    int svcIntv = 0;
    switch (index) {
    case 1: svcIntv = 2; break;
    case 2: svcIntv = 5; break;
    case 3: svcIntv = 10; break;
    case 4: svcIntv = 20; break;
    case 5: svcIntv = 30; break;
    case 6: svcIntv = 60; break;
    case 7: svcIntv = 120; break;
    default: svcIntv = (qrand() % 10 + 1); break;
    }
    return (svcIntv * 60);
}

inline int LinkDlg::fcNum(int index)
{
    int fcNum = 0;
    switch (index) {
    case 0: fcNum = 10; break;
    case 1: fcNum = 5; break;
    case 2: fcNum = 1; break;
    default: fcNum = 10; break;
    }
    return fcNum;
}

#endif // LINKDLG_H
