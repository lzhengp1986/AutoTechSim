#ifndef TYPE_H
#define TYPE_H

/* 时间戳 */
typedef struct {
    int year; /* 年(2023~) */
    int month; /* 月(1~12) */
    int day; /* 天(1~31) */
    int hour; /* 小时(1~24) */
    int min; /* 分(0~59) */
    int sec; /* 秒(0~59) */
} Time;

#endif // TYPE_H
