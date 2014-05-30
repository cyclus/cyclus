#ifndef CYCLUS_SRC_CYC_LIMITS_H_
#define CYCLUS_SRC_CYC_LIMITS_H_

#include <cmath>
#include <limits>

namespace cyclus {
/// a generic epsilon value
inline double eps() {
  return 1e-6;
}

/// an epsilon value to be used by resources
inline double eps_rsrc() {
  return 1e-6;
}

/// returns true if a double is less than 0 - eps()
inline bool IsNegative(double d) {
  return (d < (-1 * eps()));
}

/// returns true if two doubles are within eps() of one another
inline bool AlmostEq(double d1, double d2) {
  return std::fabs(d1 - d2) < eps();
}

/// maximum value for a function modifier (i.e., a_i for variable x_i)
static const double kModifierLimit = pow(10, 10);

/// maximum (+ or -) value for an integer variable
static const int kIntBoundLimit = std::numeric_limits<int>::max();

/// maximum (+ or -) value for a linear variable
static const double kLinBoundLimit = std::numeric_limits<double>::max();

/// epsilon value to turn determine difference between constraint values
static const double kConstraintEps = 1e-08;

}  // namespace cyclus

#endif  // CYCLUS_SRC_CYC_LIMITS_H_
