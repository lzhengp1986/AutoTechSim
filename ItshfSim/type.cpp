#include "type.h"

// 构造
Time::Time(void)
{
    reset();
}

void Time::reset(void)
{
    year = 2023;
    month = 1;
    day = 15;
    hour = 0;
    min = 0;
    sec = 0;
    msec = 0;
}

// 月份中天数
int Time::mdays(void)
{
    int days = 30;
    if ((month == 1) || (month == 3)
        || (month == 5) || (month == 7)
        || (month == 8) || (month == 10)
        || (month == 12)) {
        days = 31;
    }

    return days;
}

// 秒计数
int Time::second(void)
{
    /* 润年 */
    int leap = (((year % 100 != 0) && (year % 4 == 0)) || (year % 400 == 0));

    /* 秒数 */
    int md = mdays();
    int xday = (year - 2020) * (365 + leap) + month * md + day;
    int xhour = xday * 24 + hour;
    int xmin = xhour * 60 + min;
    int xsec = xmin * 60 + sec;
    return xsec;
}

