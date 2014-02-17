#ifndef COMP_MATH_H
#define COMP_MATH_H

#include "composition.h"

namespace cyclus {

/// Contains functions for performing fundamental operations on CompMap's
namespace compmath {

/// Does component-wise subtraction of the nuclide quantities of v1 and v2 and
/// returns the result.  No normalization is done. Example:
///
/// @code
/// CompMap v1;
/// v1[92235] = 2.3;
/// v1[92238] = 1.3;
/// CompMap v2;
/// v2[92235] = 1.1;
/// v2[92238] = 1.2;
/// CompMap v3 = compmath::Add(v1, v2):
/// // v3[92235] == 3.4, v3[92238] == 2.5
/// @endcode
///
CompMap Add(const CompMap& v1,
                      const CompMap& v2);

/// Does component-wise subtraction of the nuclide quantities of v1 and v2 and
/// returns the result.  No normalization is done.
CompMap Sub(const CompMap& v1, const CompMap&
                      v2);

/// All nuclides with quantities below threshold will have their quantity set to
/// zero.
void ApplyThreshold(CompMap* v, double threshold);

/// The sum of quantities of all nuclides of v is normalized to val.
void Normalize(CompMap* v, double val = 1.0);

/// Returns true if all nuclide keys in v are valid.
bool ValidNucs(const CompMap& v);

/// Returns true if all nuclides in v have quantities greater than or equal to
/// zero.
bool AllPositive(const CompMap& v);

/// Returns true if all nuclides of v1 and v2 are the same within threshold. No
/// normalization is performed.
bool AlmostEq(const CompMap& v1, const CompMap& v2, double
              threshold);

} // namespace compmath
} // namespace cyclus

#endif
