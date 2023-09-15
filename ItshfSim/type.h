#ifndef TYPE_H
#define TYPE_H

#include "macro.h"

// 时间戳
typedef struct {
    int year; /* 年(2023~) */
    int month; /* 月(1~12) */
    int day; /* 天(1~31) */
    int hour; /* 小时(0~23) */
    int min; /* 分(0~59) */
    int sec; /* 秒(0~59) */
    int msec; /* 毫秒 */
} Time;

// 状态机
typedef enum {
    WAIT, IDLE, SCAN, LINK
} SimState;

// 消息类型
typedef enum {
    MSG_FREQ_REQ = 0x1,
    MSG_FREQ_RSP,
} MsgType;

// 消息头
typedef struct {
    int type;
} MsgHead;

// 频率请求
typedef struct {
    MsgHead head;
    int fcNum; /* 请求频点个数 */
    int minChId; /* 最小信道号 */
    int maxChId; /* 最大信道号 */
} FreqReq;

// 频率响应
typedef struct {
    MsgHead head;
    int used; /* 已探测索引 */
    int total; /* 应答频点个数 */
    int glb[RSP_FREQ_NUM];
} FreqRsp;

#endif // TYPE_H
