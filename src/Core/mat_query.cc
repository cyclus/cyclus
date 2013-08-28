
#include "mat_query.h"

#include <cmath>

namespace cyclus {
MatQuery::MatQuery(Material::Ptr m) : m_(m) { };

double MatQuery::qty() {
  return m_->quantity();
};

double MatQuery::mass(Iso iso) {
  return mass_frac(iso) * qty();
}

double MatQuery::moles(Iso iso) {
  return mass(iso) / (MT->GramsPerMol(iso) * units::g);
}

double MatQuery::mass_frac(Iso iso) {
  Composition::Vect v = m_->comp()->mass_vect();
  compmath::Normalize(&v, 1);
  return v[iso];
};

double MatQuery::atom_frac(Iso iso) {
  Composition::Vect v = m_->comp()->atom_vect();
  compmath::Normalize(&v, 1);
  return v[iso];
};

bool MatQuery::AlmostEq(Material::Ptr other, double threshold) {
  bool compeq = compmath::AlmostEq(m_->comp()->mass_vect(),
                                   other->comp()->mass_vect(),
                                   threshold);
  return compeq
         && std::abs(m_->quantity() - other->quantity()) < threshold;
};
} // namespace cyclus
