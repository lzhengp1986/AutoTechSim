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
    /*! @brief 各状态处理函数，返回状态倒计时 */
    int sim_idle(const Time* ts, int& dsec);
    int sim_scan(const Time* ts, int& dsec);
    int sim_link(const Time* ts, int& dsec);
    int second(const Time* ts);

    /*! @brief 在ts上累加plus秒，保存到hist中 */
    void stamp(const Time* ts, int plus = 0);

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
