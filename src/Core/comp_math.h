#ifndef COMP_MATH_H
#define COMP_MATH_H

#include "composition.h"

namespace cyclus {

/// Contains functions for performing fundamental operations on Composition::Vect's
namespace compmath {

/// Does component-wise subtraction of the nuclide quantities of v1 and v2 and
/// returns the result.  No normalization is done. Example:
///
/// @code
/// Composition::Vect v1;
/// v1[92235] = 2.3;
/// v1[92238] = 1.3;
/// Composition::Vect v2;
/// v2[92235] = 1.1;
/// v2[92238] = 1.2;
/// Composition::Vect v3 = compmath::Add(v1, v2):
/// // v3[92235] == 3.4, v3[92238] == 2.5
/// @endcode
///
Composition::Vect Add(const Composition::Vect& v1,
                      const Composition::Vect& v2);

/// Does component-wise subtraction of the nuclide quantities of v1 and v2 and
/// returns the result.  No normalization is done.
Composition::Vect Sub(const Composition::Vect& v1, const Composition::Vect&
                      v2);

/// All nuclides with quantities below threshold will have their quantity set to
/// zero.
void ApplyThreshold(Composition::Vect* v, double threshold);

/// The sum of quantities of all nuclides of v is normalized to val.
void Normalize(cyclus::Composition::Vect* v, double val = 1.0);

/// Returns true if all nuclide keys in v are valid.
bool ValidIsos(const Composition::Vect& v);

/// Returns true if all nuclides in v have quantities greater than or equal to
/// zero.
bool AllPositive(const Composition::Vect& v);

/// Returns true if all nuclides of v1 and v2 are the same within threshold.
bool AlmostEq(const Composition::Vect& v1,
              const Composition::Vect& v2,
              double threshold);

} // namespace compmath
} // namespace cyclus

#endif
