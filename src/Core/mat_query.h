#ifndef MATQUERY_H_
#define MATQUERY_H_

#include "comp_math.h"
#include "cyc_limits.h"
#include "mass_table.h"
#include "material.h"

namespace cyclus {

class MatQuery {
 public:
  MatQuery(Material::Ptr m);

  double qty();

  double mass(Iso iso);

  double moles(Iso iso);

  double mass_frac(Iso iso);

  double atom_frac(Iso iso);

  /// Returns true if all nuclide quantities of the queries material and other
  /// are the same within threshold.
  bool AlmostEq(Material::Ptr other, double threshold = eps_rsrc());

 private:
  Material::Ptr m_;
};

} // namespace cyclus

#endif
