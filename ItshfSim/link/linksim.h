#ifndef LINKSIM_H
#define LINKSIM_H

#include "type.h"

// 基类
class State
{
public:
    int proc(const Time& t, int to);

private:
    Time m_time;
};

// idle态
class Idle : public State
{
public:

private:

};

class LinkSim
{
public:
    LinkSim(void);

private:

};

#endif // LINKSIM_H
