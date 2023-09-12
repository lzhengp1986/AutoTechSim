#ifndef LINKSIM_H
#define LINKSIM_H

#include "type.h"
#include "linkdlg.h"
#include "env/wenv.h"
#include "auto/autodlg.h"
#include <QThread>
#include <QTimer>

/* 定时器最小精度 */
#define TIMER_INTERVAL_MS 200

class LinkSim : public QThread
{
    Q_OBJECT

public:
    LinkSim(QObject *parent = nullptr);
    ~LinkSim(void);

    /* api */
    void stop(void);
    void trigger(void);
    void set_time(int year, int month);
    int simulate(int& dsec);

private:
    void setup_time(void);
    void update_time(int msec);
    void free_time(void);

    /*! @brief 各状态处理函数，返回状态倒计时 */
    int sim_idle(int& dsec);
    int sim_scan(int& dsec);
    int sim_link(int& dsec);
    int second(const Time* ts);

    /*! @brief 在ts上累加plus秒，保存到hist中 */
    void stamp(int plus = 0);

private slots:
    void on_timer_timeout(void);

signals:
    void new_time(const Time* ts);
    void new_state(int state, int dsec);
    void new_freq(int glbChId, int snr, int n0);

public:
    /* 仿真配置 */
    WEnv *m_env;
    LinkCfg *m_link;
    AutoCfg *m_auto;

private:
    /* 定时器线程 */
    Time *m_stamp;
    QTimer *m_timer;
    QThread *m_subthr;

    /* 状态机 */
    FreqReq m_req;
    FreqRsp m_rsp;
    Time m_hist;
    int m_state;
};

#endif // LINKSIM_H
