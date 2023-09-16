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
}

void LinkSim::trigger(void)
{
    /* step1.时间复位 */
    m_stamp->reset();
    int dayIndex = m_link->simDayIndex;
    int days = LinkCfg::simDays(dayIndex);
    expire(days);

    /* step2.统计值复位 */
    emit new_sts(0, 0, 0, 0);

    /* step3.算法复位 */
    sim_reset();

    /* step4.倒计时 */
    m_state = IDLE;
    stamp(10);
}

// 设置time
void LinkSim::setup_time(void)
{
    m_daily = false;
    m_stamp = new Time;
    m_to = new Time;

    /* 默认仿真1天 */
    m_expire = new Time;
    expire(1);

    /* 定时器子线程 */
    m_timer = new QTimer;
    m_subthr = new QThread(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(on_timeout()));
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

// 更新Time
void LinkSim::set_time(int year, int month)
{
    m_stamp->year = year;
    m_stamp->month = month;
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
    delete m_expire;
    delete m_to;
    m_timer = nullptr;
    m_subthr = nullptr;
    m_stamp = nullptr;
    m_expire = nullptr;
    m_to = nullptr;
}

// 定时器超时处理
void LinkSim::on_timeout(void)
{
    /* 控制速度 */
    int msec = TIMER_INTERVAL_MS;
    if (m_state != WAIT) {
        int speedIndex = m_link->tmrSpeedIndex;
        int speed = LinkCfg::timerSpeed(speedIndex);
        msec = speed * TIMER_INTERVAL_MS;
    }

    /* 秒数不变则返回 */
    m_stamp->msec += msec;
    if (m_stamp->msec < 1000) {
        return;
    }
    int md = m_stamp->mdays();

    /* 秒进位 */
    while (m_stamp->msec >= 1000) {
        m_stamp->msec -= 1000;
        m_stamp->sec++;
    }

    /* 分进位 */
    if (m_stamp->sec < 60) {
        goto SIMULATE;
    }
    while (m_stamp->sec >= 60) {
        m_stamp->sec -= 60;
        m_stamp->min++;
    }

    /* 时进位 */
    if (m_stamp->min < 60) {
        goto SIMULATE;
    }
    m_stamp->min -= 60;
    m_stamp->hour++;

    /* 天进位 */
    if (m_stamp->hour < MAX_HOUR_NUM) {
        goto SIMULATE;
    }
    m_stamp->hour -= MAX_HOUR_NUM;
    m_stamp->day++;
    m_daily = true;

    /* 月进位 */
    if (m_stamp->day <= md) {
        goto SIMULATE;
    }
    m_stamp->day -= md;
    m_stamp->month++;

    /* 年进位 */
    if (m_stamp->month <= MAX_MONTH_NUM) {
        goto SIMULATE;
    }
    m_stamp->month -= MAX_MONTH_NUM;
    m_stamp->year++;

SIMULATE:
    emit new_time(m_stamp);
    simulate();
}

// 主调度函数
void LinkSim::simulate(void)
{
    /* 每日操作 */
    if (m_daily == true) {
        m_daily = false;
        sim_reset();
    }

    /* 算法仿真 */
    int dsec = 0;
    switch (m_state) {
    case IDLE: m_state = sim_idle(dsec); break;
    case SCAN: m_state = sim_scan(dsec); break;
    case LINK: m_state = sim_link(dsec); break;
    default: break;
    }

    /* 判断过期 */
    if (isExpired()) {
        stop();
    }

    /* 更新界面状态 */
    emit new_state(m_state, dsec);
}

// idle
int LinkSim::sim_idle(int& dsec)
{
    int diff = m_to->second() - m_stamp->second();
    dsec = ABS(diff);
    if (diff > 0) {
        return IDLE;
    }

    /* 更新统计 */
    int tryFcNum = 0;
    if (m_scanNum > 0) {
        /* 向上取整 */
        int pad = m_scanNum - 1;
        tryFcNum = (m_scanFrq + pad) / m_scanNum;
    }
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
    } else if (algId == LinkCfg::MONTE_CARLO_TREE) {
        // todo
    } else if (algId == LinkCfg::ITS_HF_PROPAGATION) {
        // todo
    }

    /* 切换状态 */
    stamp(1);
    m_testNum++;
    return SCAN;
}

// scan
int LinkSim::sim_scan(int& dsec)
{
    int scanIndex = m_link->scanIntvIndex;
    int scanIntv = LinkCfg::scanIntv(scanIndex);
    int diff = m_stamp->second() - m_to->second();
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

        /* 将scan结果发到alg */
        int regret = 0;
        int type = BaseAlg::SMPL_SCAN;
        if (algId == LinkCfg::RANDOM_SEARCH) {
            regret = m_rand->notify(type, m_stamp, glbChId, out);
        } else if (algId == LinkCfg::BISECTING_SEARCH) {
            regret = m_sect->notify(type, m_stamp, glbChId, out);
        } else if (algId == LinkCfg::MONTE_CARLO_TREE) {
            regret = m_mont->notify(type, m_stamp, glbChId, out);
        } else if (algId == LinkCfg::ITS_HF_PROPAGATION) {
            regret = m_itshf->notify(type, m_stamp, glbChId, out);
        }

        /* 将scan结果发到MainWin */
        emit new_chan(glbChId, out.snr, out.n0, regret);

        /* 状态切换: LINK or SCAN */
        if (flag != ENV_OK) {
            stamp(scanIntv);
            rsp->used++;
            return SCAN;
        } else {
            if (out.isValid == true) {
                /* 统计 */
                m_scanNum++;
                m_linkNum++;

                /* 切换到link */
                int svcIndex = m_link->svcIntvIndex;
                int svcIntv = LinkCfg::svcIntv(svcIndex);
                stamp(svcIntv);
                return LINK;
            } else {
                /* 继续scan */
                stamp(scanIntv);
                rsp->used++;
                return SCAN;
            }
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
        int idleIndex = m_link->idleIntvIndex;
        int idleIntv = LinkCfg::idleIntv(idleIndex);
        stamp(idleIntv);
        return IDLE;
    }
}

// link
int LinkSim::sim_link(int& dsec)
{
    int diff = m_to->second() - m_stamp->second();
    dsec = ABS(diff);

    int algId = m_link->algIndex;
    int idleIndex = m_link->idleIntvIndex;
    int idleIntv = LinkCfg::idleIntv(idleIndex);

    /* 每分钟上报link信息 */
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

        /* 将link结果发到alg */
        int regret = 0;
        int type = BaseAlg::SMPL_LINK;
        if (algId == LinkCfg::RANDOM_SEARCH) {
            regret = m_rand->notify(type, m_stamp, glbChId, out);
        } else if (algId == LinkCfg::BISECTING_SEARCH) {
            regret = m_sect->notify(type, m_stamp, glbChId, out);
        } else if (algId == LinkCfg::MONTE_CARLO_TREE) {
            regret = m_mont->notify(type, m_stamp, glbChId, out);
        } else if (algId == LinkCfg::ITS_HF_PROPAGATION) {
            regret = m_itshf->notify(type, m_stamp, glbChId, out);
        }

        /* 将link结果发到MainWin */
        emit new_chan(glbChId, out.snr, out.n0, regret);

        /* 状态切换：信道恶化断链 */
        if ((flag != ENV_OK) || (out.isValid != true)) {
            stamp(idleIntv);
            return IDLE;
        }
    }

    /* 状态切换 */
    if (diff > 0) {
        return LINK;
    } else {
        stamp(idleIntv);
        return IDLE;
    }
}

// 每天重置算法
void LinkSim::sim_reset(void)
{
    int algId = m_link->algIndex;
    if (algId == LinkCfg::RANDOM_SEARCH) {
        m_rand->reset();
    } else if (algId == LinkCfg::BISECTING_SEARCH) {
        m_sect->reset();
    } else if (algId == LinkCfg::MONTE_CARLO_TREE) {
        m_mont->reset();
    } else if (algId == LinkCfg::ITS_HF_PROPAGATION) {
        m_itshf->reset();
    }
}

// 过期
bool LinkSim::isExpired(void)
{
    bool flag = false;
    if (m_stamp->year > m_expire->year) {
        flag = true;
    } else if (m_stamp->month > m_expire->month) {
        flag = true;
    } else if (m_stamp->day >= m_expire->day) {
        flag = true;
    }
    return flag;
}

// 在当前定时上加days
void LinkSim::expire(int days)
{
    *m_expire = *m_stamp;
    m_expire->day += days;

    /* 进位判断 */
    int md = m_stamp->mdays();
    if (m_expire->day > md) {
        m_expire->day -= md;
        m_expire->month++;
    }
    if (m_expire->month > MAX_MONTH_NUM) {
        m_expire->month -= MAX_MONTH_NUM;
        m_expire->year++;
    }
}

// 超时时戳
void LinkSim::stamp(int plus)
{
    *m_to = *m_stamp;
    m_to->sec += plus;
}

