#ifndef UTIL_H
#define UTIL_H

#include "boost/random.hpp"
typedef boost::random::mt19937 RandGen;
typedef boost::random::beta_distribution<> BetaDist;
typedef boost::random::normal_distribution<> NormDist;
typedef boost::random::bernoulli_distribution<> BernDist;
typedef boost::random::uniform_int_distribution<> UnifIntDist;

#endif // UTIL_H
