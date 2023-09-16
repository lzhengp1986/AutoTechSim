#include "linksim.h"
#include "auto/autosim.h"

LinkSim::LinkSim(QObject *parent)
    : QThread(parent)
{
    /* ITS仿真环境 */
    m_env = new WEnv;
    m_state = WAIT;

    /* link */
    m_link = new LinkCfg;

    /* 统计值 */
    m_linkNum = 0;
    m_scanNum = 0;
    m_scanNok = 0;
    m_scanFrq = 0;
    m_testNum = 0;

    /* 算法实例化 */
    setup_alg();

    /* 定时器 */
    setup_time();
}

LinkSim::~LinkSim(void)
{
    free_time();
    free_alg();
    delete m_env;
    delete m_link;
    m_env = nullptr;
    m_link = nullptr;
}

void LinkSim::stop(void)
{
    /* 统计值 */
    m_linkNum = 0;
    m_scanNum = 0;
    m_scanNok = 0;
    m_scanFrq = 0;
    m_testNum = 0;
    m_state = WAIT;

    /* 算法复位 */
    m_sect->reset();
}

void LinkSim::trigger(void)
{
    /* 统计值复位 */
    emit new_sts(0, 0, 0, 0);

    /* 倒计时 */
    stamp(10);
    m_state = IDLE;
}

// 设置time
void LinkSim::setup_time(void)
{
    m_stamp = new Time;
    m_stamp->year = 2023;
    m_stamp->month = 1;
    m_stamp->day = 1;
    m_stamp->hour = 0;
    m_stamp->min = 0;
    m_stamp->sec = 0;
    m_stamp->msec = 0;
    m_hist = *m_stamp;

    /* 定时器子线程 */
    m_timer = new QTimer;
    m_subthr = new QThread(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
    m_timer->start(TIMER_INTERVAL_MS);
    m_timer->moveToThread(m_subthr);
    m_subthr->start();
}

// 算法实例化
void LinkSim::setup_alg(void)
{
    m_rand = new BaseAlg;
    m_sect = new BisectAlg;
    m_itshf = new ItshfAlg;
    m_mont = new MonteAlg;
}

// 释放算法
void LinkSim::free_alg(void)
{
    delete m_rand;
    delete m_sect;
    delete m_itshf;
    delete m_mont;
    m_rand = nullptr;
    m_sect = nullptr;
    m_itshf = nullptr;
    m_mont = nullptr;
}

// 更新Time += msec
bool LinkSim::update_time(int msec)
{
    m_stamp->msec += msec;

    /* 毫秒进位 */
    if (m_stamp->msec >= 1000) {
        do {
            m_stamp->msec -= 1000;
            m_stamp->sec++;
        } while (m_stamp->msec >= 1000);
    } else {
        return false;
    }

    /* 秒进位 */
    if (m_stamp->sec >= 60) {
        do {
            m_stamp->sec -= 60;
            m_stamp->min++;
        } while (m_stamp->sec >= 60);
    } else {
        goto UPDATE_LABEL;
    }

    /* 分钟进位 */
    if (m_stamp->min >= 60) {
        do {
            m_stamp->min -= 60;
            m_stamp->hour++;
        } while (m_stamp->min >= 60);
    } else {
        goto UPDATE_LABEL;
    }

    /* 小时进位 */
    if (m_stamp->hour >= 24) {
        do {
            m_stamp->hour -= 24;
            m_stamp->day++;
        } while (m_stamp->hour >= 24);
    } else {
        goto UPDATE_LABEL;
    }

UPDATE_LABEL:
    emit new_time(m_stamp);
    return true;
}

// 更新Time
void LinkSim::set_time(int year, int month)
{
    m_stamp->year = year;
    m_stamp->month = month;
    m_stamp->day = 1;
    m_stamp->hour = 0;
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
    /* 控制速度 */
    int speed = 1;
    if (m_state != WAIT) {
        int speedIndex = m_link->tmrSpeedIndex;
        speed = LinkCfg::timerSpeed(speedIndex);
    }

    /* 更新时间 */
    bool flag = update_time(speed * TIMER_INTERVAL_MS);
    if (flag == false) {
        return;
    }

    /* 判断仿真天数 */
    int dayIndex = m_link->simDayIndex;
    int days = LinkCfg::simDays(dayIndex);
    if (m_stamp->day > days) {
        stop();
    }

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

    /* 更新统计 */
    int tryFcNum = (m_scanFrq + m_scanNum) / (m_scanNum + 1);
    emit new_sts(tryFcNum, m_scanFrq, m_linkNum, m_testNum);

    /* 构造频率请求消息 */
    FreqReq* req = &m_req;
    int fcNum = LinkCfg::fcNum(m_link->fcNumIndex);
    req->head.type = MSG_FREQ_REQ;
    req->fcNum = MIN(fcNum, REQ_FREQ_NUM);

    /* 调用策略推荐频率 */
    int algId = m_link->algIndex;
    if (algId == LinkCfg::RANDOM_SEARCH) {
        m_rsp = m_rand->bandit(m_req);
    } else if (algId == LinkCfg::BISECTING_SEARCH) {
        m_rsp = m_sect->bandit(m_req);
    }

    /* 切换状态 */
    stamp(1);
    m_testNum++;
    return SCAN;
}

// scan
int LinkSim::sim_scan(int& dsec)
{
    int diff = second(m_stamp) - second(&m_hist);
    int scanIntv = LinkCfg::scanIntv(m_link->scanIntvIndex);
    dsec = ABS(scanIntv - diff);
    if (diff < scanIntv) {
        return SCAN;
    }

    /* 处理1个频率 */
    FreqRsp* rsp = &m_rsp;
    int algId = m_link->algIndex;
    if (rsp->used < rsp->total) {
        EnvIn in;
        EnvOut out;
        int glbChId = rsp->glb[rsp->used];
        m_scanFrq++;

        /* 调用环境模型 */
        in.year = m_stamp->year;
        in.month = m_stamp->month;
        in.hour = m_stamp->hour;
        in.glbChId = glbChId;
        int flag = m_env->env(in, out);

        /* 将scan结果发到MainWin */
        emit new_chan(glbChId, out.snr, out.n0);

        /* 状态切换: LINK or SCAN */
        if ((flag == ENV_OK) && (out.flag == true)) {
            /* 统计 */
            m_scanNum++;
            m_linkNum++;

            /* 将scan结果发到alg */
            if (algId == LinkCfg::BISECTING_SEARCH) {
                m_sect->notify(true, glbChId, out.snr);
            }

            /* 切换到link */
            int svcIntv = LinkCfg::svcIntv(m_link->svcIntvIndex);
            stamp(svcIntv);
            return LINK;
        } else {
            /* 将scan结果发到alg */
            if (algId == LinkCfg::MONTE_CARLO_TREE) {
                // todo
            }

            stamp(scanIntv);
            rsp->used++;
            return SCAN;
        }
    } else {
        /* 统计 */
        m_scanNum++;
        m_scanNok++;

        /* 失败次数过多，复位状态 */
        if (m_scanNok >= MAX_SCAN_FAIL_THR) {
            if (algId == LinkCfg::BISECTING_SEARCH) {
                m_sect->restart();
            }
            m_scanNok = 0;
        }

        /* 超时打点 */
        int idleIntv = LinkCfg::idleIntv(m_link->idleIntvIndex);
        stamp(idleIntv);
        return IDLE;
    }
}

// link
int LinkSim::sim_link(int& dsec)
{
    int diff = second(&m_hist) - second(m_stamp);
    dsec = ABS(diff);

    /* 每分钟上报link信息 */
    int algId = m_link->algIndex;
    if (dsec % 60 == 0) {
        EnvIn in;
        EnvOut out;
        FreqRsp* rsp = &m_rsp;
        int glbChId = rsp->glb[rsp->used];

        /* 调用环境模型 */
        in.year = m_stamp->year;
        in.month = m_stamp->month;
        in.hour = m_stamp->hour;
        in.glbChId = glbChId;
        int flag = m_env->env(in, out);
        if ((flag == ENV_OK) && (out.flag == true)) {
            /* 将link结果发到MainWin */
            emit new_chan(glbChId, out.snr, out.n0);

            /* 将link结果发到alg */
            if (algId == LinkCfg::BISECTING_SEARCH) {
                m_sect->notify(true, glbChId, out.snr);
            }
        } else {
            /* 断链 */
            int idleIntv = LinkCfg::idleIntv(m_link->idleIntvIndex);
            stamp(idleIntv);
            return IDLE;
        }
    }

    /* 状态切换 */
    if (diff > 0) {
        return LINK;
    } else {
        int idleIntv = LinkCfg::idleIntv(m_link->idleIntvIndex);
        stamp(idleIntv);
        return IDLE;
    }
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

// 超时时戳
void LinkSim::stamp(int plus)
{
    m_hist = *m_stamp;
    m_hist.sec += plus;
}

