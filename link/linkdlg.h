#ifndef LINKDLG_H
#define LINKDLG_H

#include "type.h"
#include <QDialog>
#include <QRandomGenerator>

namespace Ui {
class LinkDlg;
}

class LinkCfg {
public:
    LinkCfg(void);

    int simDays(void) const;
    int timerSpeed(void) const;
    int idleIntv(void) const;
    int scanIntv(void) const;
    int svcIntv(void) const;
    int sqlRule(void) const;
    int freqNum(void) const;
    int schAlg(void) const;

public:
    int simDayIndex; /* 仿真天数 */
    int tmrSpeedIndex; /* 定时速度 */
    int idleIntvIndex; /* 业务间隔 */
    int scanIntvIndex; /* 扫频间隔 */
    int svcIntvIndex; /* 业务间隔 */
    int freqNumIndex; /* 单次请求频点个数 */
    int sqlRuleIndex; /* SQL数据选择区间 */
    int algIndex; /* 算法索引 */
};

inline int LinkCfg::simDays(void) const
{
    int days = 0;
    switch (simDayIndex) {
    case 0: days = 1; break;
    case 1: days = 2; break;
    case 2: days = 4; break;
    case 3: days = 7; break;
    case 4: days = 14; break;
    case 5: days = 21; break;
    default: days = 1; break;
    }
    return days;
}

inline int LinkCfg::timerSpeed(void) const
{
    int speed = 0;
    switch (tmrSpeedIndex) {
    case 0: speed = 32; break;
    case 1: speed = 64; break;
    case 2: speed = 128; break;
    case 3: speed = 16; break;
    case 4: speed = 8; break;
    case 5: speed = 4; break;
    default: speed = 1; break;
    }
    return speed;
}

inline int LinkCfg::scanIntv(void) const
{
    int scanIntv = 0;
    switch (scanIntvIndex) {
    case 0: scanIntv = 2; break;
    case 1: scanIntv = 1; break;
    default: scanIntv = 2; break;
    }
    return scanIntv;
}

inline int LinkCfg::svcIntv(void) const
{
    int svcIntv = 0;
    switch (svcIntvIndex) {
    case 0: svcIntv = ABS(QRandomGenerator::global()->bounded(0u, 4u) + 1); break;
    case 1: svcIntv = ABS(QRandomGenerator::global()->bounded(0u, 9u) + 1); break;
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

inline int LinkCfg::idleIntv(void) const
{
    int idleIntv = 0;
    switch (idleIntvIndex) {
    case 0: idleIntv = ABS(QRandomGenerator::global()->bounded(0u, 4u) + 1); break;
    case 1: idleIntv = ABS(QRandomGenerator::global()->bounded(0u, 9u) + 1); break;
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

inline int LinkCfg::freqNum(void) const
{
    int fcNum = 0;
    switch (freqNumIndex) {
    case 0: fcNum = 10; break;
    case 1: fcNum = 20; break;
    case 2: fcNum = 50; break;
    default: fcNum = 10; break;
    }
    return fcNum;
}

inline int LinkCfg::sqlRule(void) const
{
    int rule = 0;
    switch (sqlRuleIndex) {
    case 0: rule = SQL_WIN_1HOUR; break;
    case 1: rule = SQL_WIN_2HOUR; break;
    case 2: rule = SQL_WIN_4HOUR; break;
    case 3: rule = SQL_WIN_30MIN; break;
    default: rule = SQL_WIN_1HOUR; break;
    }

    return rule;
}

inline int LinkCfg::schAlg(void) const
{
    int alg;
    switch (algIndex) {
    case 0: alg = RANDOM_SEARCH; break;
    case 1: alg = BISECTING_SEARCH; break;
    case 2: alg = BISEC_PLUS_SEARCH; break;
    case 3: alg = MONTE_CARLO_TREE; break;
    case 4: alg = ITS_HF_PROPAGATION; break;
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
    void dlg2para(LinkCfg* cfg, QString& algName, QString& sqlRule);
    void para2dlg(const LinkCfg* cfg);

private slots:
    void on_alg_currentIndexChanged(int index);

private:
    Ui::LinkDlg *ui;
};

#endif // LINKDLG_H
