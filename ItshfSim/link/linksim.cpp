#include "linksim.h"

LinkSim::LinkSim(QObject *parent)
    : QThread(parent)
{
    /* ITS仿真环境 */
    m_env = new WEnv;

    /* link */
    m_link = new LinkCfg;
    m_link->fcNumIndex = 0; /* 10 */
    m_link->tmrSpeedIndex = 0; /* x1 */
    m_link->scanIntvIndex = 0; /* 2sec */
    m_link->svcIntvIndex = 0; /* random */

    /* auto */
    m_auto = new AutoCfg;
    m_auto->algId = 0;
    m_state = WAIT;

    /* 定时器 */
    setup_time();
}

LinkSim::~LinkSim(void)
{
    free_time();
    delete m_env;
    delete m_link;
    delete m_auto;
    m_env = nullptr;
    m_link = nullptr;
    m_auto = nullptr;
}

void LinkSim::stop(void)
{
    m_state = WAIT;
}

void LinkSim::trigger(void)
{
    stamp(30);
    m_state = IDLE;
}

// 设置time
void LinkSim::setup_time(void)
{
    m_stamp = new Time;
    m_stamp->year = 2023;
    m_stamp->month = 1;
    m_stamp->day = 1;
    m_stamp->hour = 1;
    m_stamp->min = 0;
    m_stamp->sec = 0;
    m_stamp->msec = 0;
    m_hist = *m_stamp;

    /* 定时器子线程 */
    m_timer = new QTimer;
    m_subthr = new QThread(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
    m_timer->start(1000);
    m_timer->moveToThread(m_subthr);
    m_subthr->start();
}

// 更新Time += sec
void LinkSim::update_time(int sec)
{
    m_stamp->sec += sec;
    if (m_stamp->sec < 60) {
        goto UPDATE_LABEL;
    }

    m_stamp->sec %= 60;
    m_stamp->min++;
    if (m_stamp->min < 60) {
        goto UPDATE_LABEL;
    }

    m_stamp->min %= 60;
    m_stamp->hour++;
    if (m_stamp->hour < 24) {
        goto UPDATE_LABEL;
    }

    m_stamp->hour -= 23;
    m_stamp->day++;

UPDATE_LABEL:
    emit new_time(m_stamp);
}

// 更新Time
void LinkSim::set_time(int year, int month)
{
    m_stamp->year = year;
    m_stamp->month = month;
    m_stamp->day = 1;
    m_stamp->hour = 1;
    m_stamp->min = 0;
    m_stamp->sec = 0;
    m_stamp->msec = 0;
    emit new_time(m_stamp);
}

// 释放time
void LinkSim::free_time(void)
{
    m_timer->stop();
    m_subthr->quit();
    delete m_timer;
    delete m_subthr;
    delete m_stamp;
    m_timer = nullptr;
    m_subthr = nullptr;
    m_stamp = nullptr;
}

// 定时器超时处理
void LinkSim::on_timer_timeout(void)
{
    /* 更新时间 */
    int speedIndex = m_link->tmrSpeedIndex;
    int speed = LinkDlg::timerSpeed(speedIndex);
    update_time(speed);

    /* 建链仿真 */
    int dsec = 0;
    int state = simulate(dsec);
    emit new_state(state, dsec);
}

// 主调度函数
int LinkSim::simulate(int& dsec)
{
    int nxt;
    int prv = m_state;
    switch (prv) {
    case IDLE: nxt = sim_idle(dsec); break;
    case SCAN: nxt = sim_scan(dsec); break;
    case LINK: nxt = sim_link(dsec); break;
    default: nxt = WAIT; break;
    }

    m_state = nxt;
    return prv;
}

// idle
int LinkSim::sim_idle(int& dsec)
{
    int diff = second(&m_hist) - second(m_stamp);
    dsec = ABS(diff);
    if (diff > 0) {
        return IDLE;
    }

    m_req.head.type = MSG_FREQ_REQ;
    /* call alg */
    int fcNum = LinkDlg::fcNum(m_link->fcNumIndex);
    m_rsp.head.type = MSG_FREQ_RSP;
    m_rsp.num = fcNum;
    m_rsp.fc[0] = 50;
    m_rsp.fc[1] = 1800;
    m_rsp.fc[2] = 2400;
    m_rsp.fc[3] = 3200;
    m_rsp.fc[4] = 5000;
    m_rsp.fc[5] = 250;
    m_rsp.fc[6] = 4800;
    m_rsp.fc[7] = 7400;
    m_rsp.fc[8] = 2200;
    m_rsp.fc[9] = 8000;

    /* scan时戳 */
    stamp(1);
    return SCAN;
}

// scan
int LinkSim::sim_scan(int& dsec)
{
    int diff = second(m_stamp) - second(&m_hist);
    int scanIntv = LinkDlg::scanIntv(m_link->scanIntvIndex);
    dsec = ABS(scanIntv - diff);
    if (diff < scanIntv) {
        return SCAN;
    }

    /* 为link态打时戳 */
    int svcIntv = LinkDlg::svcIntv(m_link->svcIntvIndex);
    stamp(svcIntv);
    return LINK;
}

// link
int LinkSim::sim_link(int& dsec)
{
    int diff = second(&m_hist) - second(m_stamp);
    dsec = ABS(diff);
    if (diff > 0) {
        return LINK;
    }

    /* 为idle态打时戳:<10min */
    int intv = MAX(1, qrand() % 10);
    stamp(intv * 60);
    return IDLE;
}

// 秒计数
int LinkSim::second(const Time* ts)
{
    int day = (ts->year - 2020) * 366 + ts->month * 31 + ts->day;
    int hour = day * 24 + ts->hour;
    int min = hour * 60 + ts->min;
    int sec = min * 60 + ts->sec;
    return sec;
}

// 打时戳
void LinkSim::stamp(int plus)
{
    m_hist = *m_stamp;
    m_hist.sec += plus;
}
