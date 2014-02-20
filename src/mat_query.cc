
#include "mat_query.h"

#include <cmath>

namespace cyclus {
MatQuery::MatQuery(Material::Ptr m) : m_(m) {};

double MatQuery::qty() {
  return m_->quantity();
};

double MatQuery::mass(Nuc nuc) {
  return mass_frac(nuc) * qty();
};

double MatQuery::moles(Nuc nuc) {
  return mass(nuc) / (MT->GramsPerMol(nuc) * units::g);
};

double MatQuery::mass_frac(Nuc nuc) {
  CompMap v = m_->comp()->mass();
  compmath::Normalize(&v);
  return v[nuc];
};

double MatQuery::atom_frac(Nuc nuc) {
  CompMap v = m_->comp()->atom();
  compmath::Normalize(&v);
  return v[nuc];
};

bool MatQuery::AlmostEq(Material::Ptr other, double threshold) {
  CompMap n1 = m_->comp()->mass();
  CompMap n2 = other->comp()->mass();
  compmath::Normalize(&n1);
  compmath::Normalize(&n2);
  return compmath::AlmostEq(n1, n2, threshold);
};
} // namespace cyclus
