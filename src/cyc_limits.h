#ifndef CYCLUS_SRC_CYC_LIMITS_H_
#define CYCLUS_SRC_CYC_LIMITS_H_

#include <cmath>
#include <limits>

namespace cyclus {

/// generic epsilon values
extern double cy_eps;
/// a generic epsilon value
inline double eps() {
  return cy_eps;
}

/// epsilon values to be used by resources
extern double cy_eps_rsrc;
/// an epsilon value to be used by resources
inline double eps_rsrc() {
  return cy_eps_rsrc;
}

/// returns true if a double is less than 0 - eps()
inline bool IsNegative(double d) {
  return (d < (-1 * eps()));
}

/// returns true if two doubles are within eps() of one another
inline bool AlmostEq(double d1, double d2) {
  return std::fabs(d1 - d2) < eps();
}

/// distance in ULP within which floating point numbers should be considered
/// equal.
static const double float_ulp_eq = 2;

/// maximum value for a function modifier (i.e., a_i for variable x_i)
static const double kModifierLimit = pow(10, 10);

/// maximum (+) value for an integer variable
static const int CY_LARGE_INT = std::numeric_limits<int>::max();

/// maximum (+) value for a linear variable
static const double CY_LARGE_DOUBLE = 1e299;

/// constant near-zero value
static const double CY_NEAR_ZERO = 1e-08;

}  // namespace cyclus

#endif  // CYCLUS_SRC_CYC_LIMITS_H_
