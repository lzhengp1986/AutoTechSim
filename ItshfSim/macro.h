#ifndef MACRO_H
#define MACRO_H

#define MAX_MONTH_NUM 12 /* 月数 */
#define MAX_HOUR_NUM 24 /* 小时数 */
#define MAX_SQL_SMPL 1024 /* 样本数 */

#define ONE_CHN_BW  3 /* 信道带宽 */
#define CHN_SCAN_STEP 5 /* 扫频步进 */
#define MAX_GLB_CHN 9333 /* 最大信道个数 */
#define MIN_CHN_FREQ 2000 /* 最小频率 */
#define MAX_CHN_FREQ 30000 /* 最大频率 */

#define MIN_SNR -10 /* 最小SNR */
#define MAX_SNR +30 /* 最大SNR */
#define MIN_PN0 -130 /* 最小N0 */
#define MAX_PN0 +60 /* 最大N0 */

#define BASIC_SCH_WIN 3000 /* 基本探索窗KHz */
#define REQ_FREQ_NUM 10 /* 最大请求频点数 */
#define RSP_FREQ_NUM REQ_FREQ_NUM /* 最大响应频点数 */

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) > 0 ? (a) : -(a))
#define GLB2FREQ(g) ((g) * ONE_CHN_BW + MIN_CHN_FREQ)

#endif // MACRO_H
