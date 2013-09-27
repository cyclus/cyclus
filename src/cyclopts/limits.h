#ifndef CYCLUS_CYCLOPTS_CYCLUS_CYCLOPTS_LIMITS_H_
#define CYCLUS_CYCLOPTS_CYCLUS_CYCLOPTS_LIMITS_H_

#include <cmath>
#include <limits>

namespace cyclus {
namespace cyclopts {
/// maximum value for a function modifier (i.e., a_i for variable x_i)
static const double kModifierLimit = pow(10,10);

/// maximum (+ or -) value for an integer variable
static const int kIntBoundLimit = std::numeric_limits<int>::max();

/// maximum (+ or -) value for an linear variable
static const double kLinBoundLimit = std::numeric_limits<double>::max();

/// epsilon value to turn determine difference between constraint values
static const double kConstraintEps = 1e-08;
} // namespace cyclopts
} // namespace cyclus

#endif
