#include "mat_query.h"
#include "pyne.h"

#include <cmath>

namespace cyclus {
namespace toolkit {

MatQuery::MatQuery(Material::Ptr m) : m_(m) {}

double MatQuery::qty() {
  return m_->quantity();
}

double MatQuery::mass(Nuc nuc) {
  return mass_frac(nuc) * qty();
}

double MatQuery::moles(Nuc nuc) {
  return mass(nuc) / (pyne::atomic_mass(nuc) * units::g);
}

double MatQuery::mass_frac(Nuc nuc) {
  CompMap v = m_->comp()->mass();
  compmath::Normalize(&v);
  return v[nuc];
}

double MatQuery::atom_frac(Nuc nuc) {
  CompMap v = m_->comp()->atom();
  compmath::Normalize(&v);
  return v[nuc];
}

double MatQuery::mass(std::string nuc) {
  return mass(pyne::nucname::id(nuc));
}

double MatQuery::moles(std::string nuc) {
  return moles(pyne::nucname::id(nuc));
}

double MatQuery::mass_frac(std::string nuc) {
  return mass_frac(pyne::nucname::id(nuc));
}

double MatQuery::atom_frac(std::string nuc) {
  return atom_frac(pyne::nucname::id(nuc));
}

bool MatQuery::AlmostEq(Material::Ptr other, double threshold) {
  CompMap n1 = m_->comp()->mass();
  CompMap n2 = other->comp()->mass();
  compmath::Normalize(&n1);
  compmath::Normalize(&n2);
  return compmath::AlmostEq(n1, n2, threshold);
}

double MatQuery::Amount(Composition::Ptr c) {
  CompMap m = m_->comp()->mass();
  CompMap m_other = c->mass();

  compmath::Normalize(&m);
  compmath::Normalize(&m_other);

  Nuc limiter;
  double min_ratio = 1e300;
  CompMap::iterator it;
  for (it = m_other.begin(); it != m_other.end(); ++it) {
    Nuc nuc = it->first;
    double qty_other = it->second;
    if (m.count(nuc) == 0 && qty_other > 0) {
      return 0;
    }
    double qty = m[nuc];

    double ratio = qty / qty_other;
    if (ratio < min_ratio) {
      min_ratio = ratio;
      limiter = nuc;
    }
  }

  double mult = min_ratio * qty();
  compmath::Normalize(&m_other, mult);
  double sum = 0;
  for (it = m_other.begin(); it != m_other.end(); ++it) {
    sum += it->second;
  }
  return sum;
}

}  // namespace toolkit
}  // namespace cyclus
