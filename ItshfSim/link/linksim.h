#ifndef LINKSIM_H
#define LINKSIM_H

#include "type.h"
#include "linkdlg.h"
#include "env/wenv.h"
#include "auto/autodlg.h"

class LinkSim
{
public:
    LinkSim(void);
    ~LinkSim(void);

    /* api */
    void quit(void);
    void start(const Time* ts);
    int simulate(const Time* ts, int& dsec);

private:
    void sim_idle(const Time* ts, int& dsec);
    void sim_scan(const Time* ts, int& dsec);
    void sim_link(const Time* ts, int& dsec);
    int second(const Time* ts);

public:
    /* 仿真配置 */
    WEnv *m_env;
    LinkCfg *m_link;
    AutoCfg *m_auto;

private:
    /* 状态机 */
    FreqReq m_req;
    FreqRsp m_rsp;
    Time m_hist;
    int m_state;
};

inline void LinkSim::quit(void)
{
    m_state = WAIT;
}

inline void LinkSim::start(const Time* ts)
{
    m_hist = *ts;
    m_state = IDLE;
}

#endif // LINKSIM_H
