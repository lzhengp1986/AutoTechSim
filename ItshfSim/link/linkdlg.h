#ifndef LINKDLG_H
#define LINKDLG_H

#include "macro.h"
#include <QDialog>

namespace Ui {
class LinkDlg;
}

class LinkCfg {
public:
    LinkCfg(void);

    static int simDays(int index);
    static int timerSpeed(int index);
    static int idleIntv(int index);
    static int scanIntv(int index);
    static int svcIntv(int index);
    static int fcNum(int index);
    static int algorithm(int index);

    /* 算法类型 */
    enum {
        RANDOM_SEARCH = 0,
        BISECTING_SEARCH,
        MONTE_CARLO_TREE,
        ITS_HF_PROPAGATION
    };

public:
    int simDayIndex; /* 仿真天数 */
    int tmrSpeedIndex; /* 定时速度 */
    int idleIntvIndex; /* 业务间隔 */
    int scanIntvIndex; /* 扫频间隔 */
    int svcIntvIndex; /* 业务间隔 */
    int fcNumIndex; /* 单次请求频点个数 */
    int algIndex; /* 算法索引 */
};

inline int LinkCfg::simDays(int index)
{
    int days = 0;
    switch (index) {
    case 0: days = 1; break;
    case 1: days = 2; break;
    case 2: days = 4; break;
    case 3: days = 15; break;
    case 4: days = 30; break;
    default: days = 1; break;
    }
    return days;
}

inline int LinkCfg::timerSpeed(int index)
{
    int speed = 0;
    switch (index) {
    case 0: speed = 256; break;
    case 1: speed = 128; break;
    case 2: speed = 64; break;
    case 3: speed = 8; break;
    case 4: speed = 4; break;
    default: speed = 1; break;
    }
    return speed;
}

inline int LinkCfg::scanIntv(int index)
{
    int scanIntv = 0;
    switch (index) {
    case 0: scanIntv = 2; break;
    case 1: scanIntv = 1; break;
    default: scanIntv = 2; break;
    }
    return scanIntv;
}

inline int LinkCfg::svcIntv(int index)
{
    int svcIntv = 0;
    switch (index) {
    case 0: svcIntv = ABS(qrand() % 5 + 1); break;
    case 1: svcIntv = ABS(qrand() % 10 + 1); break;
    case 2: svcIntv = 2; break;
    case 3: svcIntv = 5; break;
    case 4: svcIntv = 10; break;
    case 5: svcIntv = 20; break;
    case 6: svcIntv = 30; break;
    case 7: svcIntv = 60; break;
    case 8: svcIntv = 120; break;
    default: svcIntv = 2; break;
    }
    return (svcIntv * 60);
}

inline int LinkCfg::idleIntv(int index)
{
    int idleIntv = 0;
    switch (index) {
    case 0: idleIntv = ABS(qrand() % 5 + 1); break;
    case 1: idleIntv = ABS(qrand() % 10 + 1); break;
    case 2: idleIntv = 1; break;
    case 3: idleIntv = 2; break;
    case 4: idleIntv = 5; break;
    case 5: idleIntv = 10; break;
    case 6: idleIntv = 20; break;
    case 7: idleIntv = 30; break;
    case 8: idleIntv = 60; break;
    case 9: idleIntv = 120; break;
    case 10: idleIntv = 240; break;
    default: idleIntv = 1; break;
    }
    return (idleIntv * 60);
}

inline int LinkCfg::fcNum(int index)
{
    int fcNum = 0;
    switch (index) {
    case 0: fcNum = 10; break;
    case 1: fcNum = 5; break;
    default: fcNum = 10; break;
    }
    return fcNum;
}

inline int LinkCfg::algorithm(int index)
{
    int alg;
    switch (index) {
    case 0: alg = RANDOM_SEARCH; break;
    case 1: alg = BISECTING_SEARCH; break;
    case 2: alg = MONTE_CARLO_TREE; break;
    case 3: alg = ITS_HF_PROPAGATION; break;
    default: alg = RANDOM_SEARCH; break;
    }
    return alg;
}

class LinkDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LinkDlg(QWidget *parent = nullptr);
    ~LinkDlg();

    /* api */
    void dlg2para(LinkCfg* cfg);
    void para2dlg(const LinkCfg* cfg);

private:
    Ui::LinkDlg *ui;
};

#endif // LINKDLG_H
