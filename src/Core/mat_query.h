#if !defined(_MATQUERY_H)
#define _MATQUERY_H

#include "material.h"
#include "mass_table.h"
#include "comp_math.h"

namespace cyclus {

class MatQuery {
 public:
  MatQuery(Material::Ptr m) : m_(m) { }

  double qty() {
    return m_->quantity();
  };

  double mass(Iso iso) {
    return mass_frac(iso) * qty();
  }

  double moles(Iso iso) {
    return mass(iso) / (MT->GramsPerMol(iso) * units::g);
  }

  double mass_frac(Iso iso) {
    Composition::Vect v = m_->comp()->mass_vect();
    compmath::Normalize(&v, 1);
    return v[iso];
  };

  double atom_frac(Iso iso) {
    Composition::Vect v = m_->comp()->atom_vect();
    compmath::Normalize(&v, 1);
    return v[iso];
  };

  /// returns the mass (kg) of composition c that can be extracted from
  /// this material.
  double Contains(Composition::Ptr c) {
    Composition::Vect v = m_->comp()->mass_vect();
    Composition::Vect cv = c->mass_vect();
    compmath::Normalize(&v, 1);
    compmath::Normalize(&cv, 1);

    double minRatio = -1;
    Composition::Vect::iterator it;
    for (it = cv.begin(); it != cv.end(); ++it) {
      Iso iso = it->first;
      if (cv[iso] <= 0) {
        continue;
      }

      double ratio = v[iso] / cv[iso];
      if (ratio < minRatio || minRatio < 0) {
        minRatio = ratio;
      }
    }
    return minRatio * m_->quantity();
  };

 private:
  Material::Ptr m_;
};

} // namespace cyclus

#endif
