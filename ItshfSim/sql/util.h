#ifndef BETA_H
#define BETA_H

/* 参考《常用算法程序集.第五版》 */

/*! @brief 只生成1个随机数 */
extern int rab1(int a, int b, int *r);
/*! @brief 生成1个正态分布 */
extern double grn1(double u, double g, double *r);
/*! @brief 使用抽样法生成近似beta分布随机数 */
extern double rbeta(double a, double b, double *r);

#endif // BETA_H
