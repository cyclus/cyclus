#ifndef CYCLOPTS_CYCLOPTS_LIMITS_H_
#define CYCLOPTS_CYCLOPTS_LIMITS_H_

#include <cmath>

namespace cyclopts
{
/// maximum value for a function modifier (i.e., a_i for variable x_i)
static const double kModifierLimit = pow(10,10);

/// epsilon value to turn determine difference between constraint values
static const double kConstraintEps = 1e-08;
}

#endif
