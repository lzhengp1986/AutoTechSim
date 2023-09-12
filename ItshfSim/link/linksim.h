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
    int sim_idle(const Time* ts, int& dsec);
    int sim_scan(const Time* ts, int& dsec);
    int sim_link(const Time* ts, int& dsec);
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

#endif // LINKSIM_H
