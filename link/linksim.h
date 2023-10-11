#ifndef LINKSIM_H
#define LINKSIM_H

#include "type.h"
#include "linkdlg.h"
#include "env/wenv.h"
#include "auto/autosim.h"

#include <QThread>
#include <QTimer>

/* 定时器最小精度 */
#define TIMER_INTERVAL_MS 5

class LinkSim : public QThread
{
    Q_OBJECT

public:
    LinkSim(QObject *parent = nullptr);
    ~LinkSim(void);

    /* api */
    void deactive(void);
    void trigger(void);
    void set_time(int year, int month);

private:
    void setup_time(void);
    void free_time(void);

    void setup_alg(void);
    void free_alg(void);

    /*! @brief 算法仿真 */
    int sim_idle(const Time* ts, int& dsec);
    int sim_req(const Time* ts, int& dsec);
    int sim_scan(const Time* ts, int& dsec);
    int sim_link(const Time* ts, int& dsec);
    void sim_reset(void);

    /* 工具函数 */
    float avgScan(void);
    void expire(int days);
    bool isExpired(const Time* ts);
    void stamp(const Time* ts, int plus);

    /* 任务队列 */
    bool isempty(void);
    void push(const Time* ts);
    Time pop(void);

private slots:
    void on_timeout(void);

signals:
    void new_day(void);
    void new_time(const Time* ts);
    void new_state(int state, int dsec);
    void new_chan(int type, int index, int glbChId, int snr, int n0, int regret);
    void new_sts(float avgScan, int scanNum, int linkNum, int testNum);

public:
    /* 仿真配置 */
    WEnv *m_env;
    LinkCfg *m_link;

protected:
    virtual void run(void);

private:
    /* 定时器线程 */
    bool m_daily;
    QThread *m_subthr;
    QTimer *m_timer; /* 定时器 */
    Time *m_expire; /* 过期时间 */
    Time *m_to; /* 定时器时间 */

    /* 算法实例化 */
    BaseAlg *m_alg;
    SimSql *m_sql;

    /* 状态机 */
    FreqReq m_req;
    FreqRsp m_rsp;
    int m_state; /* 仿真状态 */
    Time *m_ts; /* 仿真时间 */
    Time *m_te; /* 结束时间 */
    bool m_active; /* 主线程状态 */

    /* 统计值 */
    unsigned m_linkNum; /* 建链总次数 */
    unsigned m_scanFrq; /* 总扫频频点数 */
    unsigned m_scanNok; /* 扫频失败次数 */
    unsigned m_scanNum; /* 扫频总次数 */
    unsigned m_testNum; /* 测试总次数 */
};

#endif // LINKSIM_H
