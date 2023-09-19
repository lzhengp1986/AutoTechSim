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
    /* step1.统计值复位 */
    emit new_sts(0, 0, 0, 0);

    /* step2.sql复位 */
    m_sql->drop(BaseAlg::SMPL_SCAN);
    m_sql->drop(BaseAlg::SMPL_LINK);

    /* step3.算法复位 */
    sim_reset();

    /* step4.时间复位 */
    m_to->reset();
    m_ts->reset();
    expire(m_link->simDays());

    /* step5.倒计时 */
    m_state = IDLE;
    stamp(10);
}

// 设置time
void LinkSim::setup_time(void)
{
    m_daily = false;
    m_to = new Time;
    m_te = new Time;
    m_ts = new Time;

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
    m_sql = new SimSql;
}

// 释放算法
void LinkSim::free_alg(void)
{
    delete m_rand;
    delete m_sect;
    delete m_itshf;
    delete m_mont;
    delete m_sql;
    m_rand = nullptr;
    m_sect = nullptr;
    m_itshf = nullptr;
    m_mont = nullptr;
    m_sql = nullptr;
}

// 更新Time
void LinkSim::set_time(int year, int month)
{
    m_to->year = year;
    m_to->month = month;
    *m_ts = *m_to;
    emit new_time(m_ts);
}

// 释放time
void LinkSim::free_time(void)
{
    m_timer->stop();
    m_subthr->quit();
    delete m_timer;
    delete m_subthr;
    delete m_expire;
    delete m_to;
    delete m_te;
    delete m_ts;
    m_timer = nullptr;
    m_subthr = nullptr;
    m_expire = nullptr;
    m_to = nullptr;
    m_te = nullptr;
    m_ts = nullptr;
}

// 定时器超时处理
void LinkSim::on_timeout(void)
{
    /* 控制速度 */
    int msec = TIMER_INTERVAL_MS;
    if (m_state != WAIT) {
        int speed = m_link->timerSpeed();
        msec = speed * TIMER_INTERVAL_MS;
    }

    /* 秒数不变则返回 */
    m_to->msec += msec;
    if (m_to->msec < 1000) {
        return;
    }
    int md = m_to->mdays();

    /* 秒进位 */
    while (m_to->msec >= 1000) {
        m_to->msec -= 1000;
        m_to->sec++;
    }

    /* 分进位 */
    if (m_to->sec < 60) {
        goto SIMULATE;
    }
    while (m_to->sec >= 60) {
        m_to->sec -= 60;
        m_to->min++;
    }

    /* 时进位 */
    if (m_to->min < 60) {
        goto SIMULATE;
    }
    m_to->min -= 60;
    m_to->hour++;

    /* 天进位 */
    if (m_to->hour < MAX_HOUR_NUM) {
        goto SIMULATE;
    }
    m_to->hour -= MAX_HOUR_NUM;
    m_to->day++;
    m_daily = true;
    emit new_day();

    /* 月进位 */
    if (m_to->day <= md) {
        goto SIMULATE;
    }
    m_to->day -= md;
    m_to->month++;

    /* 年进位 */
    if (m_to->month <= MAX_MONTH_NUM) {
        goto SIMULATE;
    }
    m_to->month -= MAX_MONTH_NUM;
    m_to->year++;

SIMULATE:
    *m_ts = *m_to;
    emit new_time(m_ts);
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
    int diff = m_te->second() - m_ts->second();
    dsec = ABS(diff);
    if (diff > 0) {
        return IDLE;
    }

    /* 更新统计 */
    int avgScanFreq = avgScan();
    emit new_sts(avgScanFreq, m_scanFrq, m_linkNum, m_testNum);

    /* 构造频率请求消息 */
    FreqReq* req = &m_req;
    int fcNum = m_link->freqNum();
    req->head.type = MSG_FREQ_REQ;
    req->fcNum = MIN(fcNum, REQ_FREQ_NUM);

    /* 调用策略推荐频率 */
    int algId = m_link->recAlg();
    int sqlRule = m_link->sqlRule();
    SqlIn ain = SqlIn(m_ts, m_sql, sqlRule);
    if (algId == RANDOM_SEARCH) {
        m_rsp = m_rand->bandit(ain, m_req);
    } else if (algId == BISECTING_SEARCH) {
        m_rsp = m_sect->bandit(ain, m_req);
    } else if (algId == MONTE_CARLO_TREE) {
        m_rsp = m_mont->bandit(ain, m_req);
    } else if (algId == ITS_HF_PROPAGATION) {
        m_rsp = m_itshf->bandit(ain, m_req);
    }

    /* 切换状态 */
    stamp(1);
    m_testNum++;
    return SCAN;
}

// scan
int LinkSim::sim_scan(int& dsec)
{
    int scanIntv = m_link->scanIntv();
    int diff = m_ts->second() - m_te->second();
    dsec = ABS(scanIntv - diff);
    if (diff < scanIntv) {
        return SCAN;
    }

    int algId = m_link->recAlg();
    int sqlRule = m_link->sqlRule();
    SqlIn ain = SqlIn(m_ts, m_sql, sqlRule);

    /* 处理1个频率 */
    FreqRsp* rsp = &m_rsp;
    if (rsp->used < rsp->total) {
        int glbChId = rsp->glb[rsp->used];
        m_scanFrq++;
        EnvOut out;

        /* 调用环境模型 */
        EnvIn ein = EnvIn(m_ts, glbChId);
        int flag = m_env->env(ein, out);

        /* 将scan结果记录到sql */
        int type = BaseAlg::SMPL_SCAN;
        int ret = m_sql->insert(type, m_ts, out.isValid, glbChId, out.snr, out.n0);
        if (ret != SQLITE_OK) {
            /* nothing-to-do */
        }

        /* 将scan结果发到alg */
        int regret = 0;
        if (algId == RANDOM_SEARCH) {
            regret = m_rand->notify(ain, glbChId, out);
        } else if (algId == BISECTING_SEARCH) {
            regret = m_sect->notify(ain, glbChId, out);
        } else if (algId == MONTE_CARLO_TREE) {
            regret = m_mont->notify(ain, glbChId, out);
        } else if (algId == ITS_HF_PROPAGATION) {
            regret = m_itshf->notify(ain, glbChId, out);
        }

        /* 将scan结果发到MainWin */
        emit new_chan(SCAN, glbChId, out.snr, out.n0, regret);

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
                stamp(m_link->svcIntv());
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
            if (algId == RANDOM_SEARCH) {
                m_rand->restart(ain);
            } else if (algId == BISECTING_SEARCH) {
                m_sect->restart(ain);
            } else if (algId == MONTE_CARLO_TREE) {
                m_mont->restart(ain);
            } else if (algId == ITS_HF_PROPAGATION) {
                m_itshf->restart(ain);
            }
            m_scanNok = 0;
        }

        /* 超时打点 */
        stamp(m_link->idleIntv());
        return IDLE;
    }
}

// link
int LinkSim::sim_link(int& dsec)
{
    int diff = m_te->second() - m_ts->second();
    dsec = ABS(diff);

    int idleIntv = m_link->idleIntv();

    /* 每分钟上报link信息 */
    if (dsec % 60 == 0) {
        FreqRsp* rsp = &m_rsp;
        int glbChId = rsp->glb[rsp->used];
        EnvOut out;

        /* 调用环境模型 */
        EnvIn in = EnvIn(m_ts, glbChId);
        int flag = m_env->env(in, out);

        /* 将link结果记录到sql */
        int type = BaseAlg::SMPL_LINK;
        int ret = m_sql->insert(type, m_ts, out.isValid, glbChId, out.snr, out.n0);
        if (ret != SQLITE_OK) {
            /* nothing-to-do */
        }

        /* 将link结果发到alg */
        int regret = 0;
        int algId = m_link->recAlg();
        int sqlRule = m_link->sqlRule();
        SqlIn ain = SqlIn(m_ts, m_sql, sqlRule);
        if (algId == RANDOM_SEARCH) {
            regret = m_rand->notify(ain, glbChId, out);
        } else if (algId == BISECTING_SEARCH) {
            regret = m_sect->notify(ain, glbChId, out);
        } else if (algId == MONTE_CARLO_TREE) {
            regret = m_mont->notify(ain, glbChId, out);
        } else if (algId == ITS_HF_PROPAGATION) {
            regret = m_itshf->notify(ain, glbChId, out);
        }

        /* 将link结果发到MainWin */
        emit new_chan(LINK, glbChId, out.snr, out.n0, regret);

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
    int algId = m_link->recAlg();
    if (algId == RANDOM_SEARCH) {
        m_rand->reset();
    } else if (algId == BISECTING_SEARCH) {
        m_sect->reset();
    } else if (algId == MONTE_CARLO_TREE) {
        m_mont->reset();
    } else if (algId == ITS_HF_PROPAGATION) {
        m_itshf->reset();
    }
}

// 过期
bool LinkSim::isExpired(void)
{
    bool flag = false;
    if (m_ts->year > m_expire->year) {
        flag = true;
    } else if (m_ts->month > m_expire->month) {
        flag = true;
    } else if (m_ts->day >= m_expire->day) {
        flag = true;
    }
    return flag;
}

int LinkSim::avgScan(void)
{
    int avg = 0;

    /* 向上取整 */
    if (m_scanNum > 0) {
        avg = (m_scanFrq + m_scanNum - 1) / m_scanNum;
    }

    return avg;
}

// 在当前定时上加days
void LinkSim::expire(int days)
{
    *m_expire = *m_ts;
    m_expire->day += days;

    /* 进位判断 */
    int md = m_ts->mdays();
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
    *m_te = *m_ts;
    m_te->sec += plus;
}

