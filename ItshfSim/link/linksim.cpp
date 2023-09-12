#include "linksim.h"

LinkSim::LinkSim(void)
{
    /* ITS仿真环境 */
    m_env = new WEnv;

    /* link */
    m_link = new LinkCfg;
    m_link->fcNumIndex = 0; /* 10 */
    m_link->tmrSpeedIndex = 0; /* x1 */
    m_link->scanIntvIndex = 0; /* 2sec */
    m_link->svcIntvIndex = 0; /* random */
    memset(&m_hist, 0, sizeof(Time));
    m_hist.year = 2023;

    /* auto */
    m_auto = new AutoCfg;
    m_auto->algId = 0;
    m_state = WAIT;
}

LinkSim::~LinkSim(void)
{
    delete m_env;
    delete m_link;
    delete m_auto;
    m_env = nullptr;
    m_link = nullptr;
    m_auto = nullptr;
}

void LinkSim::quit(void)
{
    m_state = WAIT;
}

void LinkSim::start(const Time* ts)
{
    m_hist = *ts;
    m_state = IDLE;
    int svcIntv = LinkDlg::svcIntv(m_link->svcIntvIndex);
    m_hist.sec += svcIntv;
}

// 主调度函数
int LinkSim::simulate(const Time* ts, int& dsec)
{
    int next;
    switch (m_state) {
    case IDLE: next = sim_idle(ts, dsec); break;
    case SCAN: next = sim_scan(ts, dsec); break;
    case LINK: next = sim_link(ts, dsec); break;
    default: next = WAIT; break;
    }

    return m_state = next;
}

// idle
int LinkSim::sim_idle(const Time* ts, int& dsec)
{
    int diff = second(&m_hist) - second(ts);
    dsec = ABS(diff);
    if (diff > 0) {
        return IDLE;
    }

    m_hist = *ts;
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
    return SCAN;
}

// scan
int LinkSim::sim_scan(const Time* ts, int& dsec)
{
    int diff = second(ts) - second(&m_hist);
    int scanIntv = LinkDlg::scanIntv(m_link->scanIntvIndex);
    dsec = ABS(scanIntv - diff);
    if (diff < scanIntv) {
        return SCAN;
    }

    m_hist = *ts;
    int svcIntv = LinkDlg::svcIntv(m_link->svcIntvIndex);
    m_hist.sec += ABS(10 + qrand() % svcIntv);
    return LINK;
}

// link
int LinkSim::sim_link(const Time* ts, int& dsec)
{
    int diff = second(&m_hist) - second(ts);
    dsec = ABS(diff);
    if (diff > 0) {
        return LINK;
    }

    m_hist = *ts;
    int svcIntv = LinkDlg::svcIntv(m_link->svcIntvIndex);
    m_hist.sec += svcIntv;
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
