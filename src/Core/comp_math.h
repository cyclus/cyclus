#ifndef COMP_MATH_H
#define COMP_MATH_H

#include "composition.h"

namespace cyclus {
namespace compmath {

Composition::Vect Add(const Composition::Vect& v1, double qty1,
                      const Composition::Vect& v2, double qty2);

Composition::Vect Sub(const Composition::Vect& v1, double qty1,
                       const Composition::Vect& v2, double qty2);

void ApplyThreshold(Composition::Vect* v, double threshold);

void Normalize(cyclus::Composition::Vect* v, double val);

bool ValidIsos(const Composition::Vect& v);

bool AllPositive(const Composition::Vect& v);

bool AlmostEq(const Composition::Vect& v1,
              const Composition::Vect& v2,
              double threshold);

} // namespace compmath
} // namespace cyclus

#endif
