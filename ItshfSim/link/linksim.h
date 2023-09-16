#ifndef LINKSIM_H
#define LINKSIM_H

#include "macro.h"
#include "type.h"
#include "linkdlg.h"
#include "env/wenv.h"
#include "auto/autosim.h"

#include <QThread>
#include <QTimer>

/* 定时器最小精度 */
#define TIMER_INTERVAL_MS 10

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
    bool update_time(int msec);
    void free_time(void);

    void setup_alg(void);
    void free_alg(void);

    /*! @brief 各状态处理函数，返回状态倒计时 */
    int sim_idle(int& dsec);
    int sim_scan(int& dsec);
    int sim_link(int& dsec);
    int second(const Time* ts);
    void stamp(int plus);

private slots:
    void on_timer_timeout(void);

signals:
    void new_time(const Time* ts);
    void new_state(int state, int dsec);
    void new_chan(int glbChId, int snr, int n0);
    void new_sts(int scanTry, int scanNum, int linkNum, int testNum);

public:
    /* 仿真配置 */
    WEnv *m_env;
    LinkCfg *m_link;

private:
    /* 定时器线程 */
    Time *m_stamp;
    QTimer *m_timer;
    QThread *m_subthr;

    /* 算法实例化 */
    BaseAlg *m_rand;
    BisectAlg *m_sect;
    ItshfAlg *m_itshf;
    MonteAlg *m_mont;

    /* 状态机 */
    FreqReq m_req;
    FreqRsp m_rsp;
    Time m_hist;
    int m_state;

    /* 统计值 */
    unsigned m_linkNum; /* 建链总次数 */
    unsigned m_scanFrq; /* 总扫频频点数 */
    unsigned m_scanNok; /* 扫频失败次数 */
    unsigned m_scanNum; /* 扫频总次数 */
    unsigned m_testNum; /* 测试总次数 */
};

#endif // LINKSIM_H
