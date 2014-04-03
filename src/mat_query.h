#ifndef CYCLUS_SRC_MAT_QUERY_H_
#define CYCLUS_SRC_MAT_QUERY_H_

#include "comp_math.h"
#include "cyc_limits.h"
#include "material.h"
#include "pyne.h"

namespace cyclus {

/// A class that provides convenience methods for querying a material's properties.
class MatQuery {
 public:
  /// Creates a new query object inspecting m.
  MatQuery(Material::Ptr m);

  /// Returns the mass in kg of the material.
  double qty();

  /// Returns the mass in kg of nuclide nuc in the material.
  double mass(Nuc nuc);

  /// Returns the number of moles of nuclide nuc in the material.
  double moles(Nuc nuc);

  /// Returns the mass fraction of nuclide nuc in the material.
  double mass_frac(Nuc nuc);

  /// returns the atom/mole fraction of nuclide nuc in the material.
  double atom_frac(Nuc nuc);

  /// Returns true if all nuclide fractions of the material and other
  /// are the same within threshold.
  bool AlmostEq(Material::Ptr other, double threshold = eps_rsrc());

  /// Returns the maximum mass quantity of composition c that can be extracted
  /// from the material.
  double Amount(Composition::Ptr c);

 private:
  Material::Ptr m_;
};

} // namespace cyclus

#endif  // CYCLUS_SRC_MAT_QUERY_H_
