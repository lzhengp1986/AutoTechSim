#include "type.h"

// 初始日期
#define INIT_YEAR  2023
#define INIT_MONTH 1
#define INIT_DAY   1

// 构造
Time::Time(void)
{
    year = INIT_YEAR;
    month = INIT_MONTH;
    reset();
}

void Time::reset(void)
{
    /* 除开年月 */
    day = INIT_DAY;
    hour = 0;
    min = 0;
    sec = 0;
    msec = 0;
}

float Time::Hour(void) const
{
    float fm = min / 60.0f;
    float fs = sec / 3600.0f;
    float hr = hour + fm + fs;
    return hr;
}

// 月份中天数
int Time::mdays(void) const
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
int Time::second(void) const
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

