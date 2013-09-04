#ifndef MATQUERY_H_
#define MATQUERY_H_

#include "comp_math.h"
#include "cyc_limits.h"
#include "mass_table.h"
#include "material.h"

namespace cyclus {

/// A class that provides convenience methods for querying a material's properties.
class MatQuery {
 public:
  /// Creates a new query object inspecting m.
  MatQuery(Material::Ptr m);

  /// Returns the mass in kg of the material.
  double qty();

  /// Returns the mass in kg of isotope iso in the material.
  double mass(Iso iso);

  /// Returns the number of moles of isotope iso in the material.
  double moles(Iso iso);

  /// Returns the mass fraction of isotope iso in the material.
  double mass_frac(Iso iso);

  /// returns the atom/mole fraction of isotope iso in the material.
  double atom_frac(Iso iso);

  /// Returns true if all nuclide fractions of the material and other
  /// are the same within threshold.
  bool AlmostEq(Material::Ptr other, double threshold = eps_rsrc());

 private:
  Material::Ptr m_;
};

} // namespace cyclus

#endif
