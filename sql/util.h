#ifndef UTIL_H
#define UTIL_H

#include "boost/random.hpp"
typedef boost::random::mt19937 RandGen;
typedef boost::random::beta_distribution<> BetaDist;

/* 参考《常用算法程序集.第五版》 */

/*! @brief 只生成1个随机数 */
extern int rab1(int a, int b, int *r);
/*! @brief 生成1个正态分布 */
extern double grn1(double u, double g, double *r);

#endif // UTIL_H
