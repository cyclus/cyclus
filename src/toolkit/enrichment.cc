#include "enrichment.h"

#include <cmath>
#include <sstream>

#include "error.h"
#include "cyc_limits.h"
#include "logger.h"
#include "mat_query.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Assays::Assays(double feed, double product, double tails)
    : feed_(feed), product_(product), tails_(tails) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Assays::Feed() const {
  return feed_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Assays::Product() const {
  return product_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Assays::Tails() const {
  return tails_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double UraniumAssay(Material::Ptr rsrc) {
  return UraniumAssayAtom(rsrc);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double UraniumAssayAtom(Material::Ptr rsrc) {
  double value;
  MatQuery mq(rsrc);
  double u235 = mq.atom_frac(922350000);
  double u238 = mq.atom_frac(922380000);

  LOG(LEV_DEBUG1, "CEnr") << "Comparing u235 atom fraction : " << u235
                          << " with u238 atom fraction: " << u238;

  if (u235 + u238 > 0) {
    value = u235 / (u235 + u238);
  } else {
    value = 0;
  }
  return value;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double UraniumAssayMass(Material::Ptr rsrc) {
  double value;
  MatQuery mq(rsrc);
  double u235 = mq.mass_frac(922350000);
  double u238 = mq.mass_frac(922380000);

  LOG(LEV_DEBUG1, "CEnr") << "Comparing u235 mass fraction : " << u235
                          << " with u238 mass fraction: " << u238;

  if (u235 + u238 > 0) {
    value = u235 / (u235 + u238);
  } else {
    value = 0;
  }
  return value;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double UraniumQty(Material::Ptr rsrc) {
  MatQuery mq(rsrc);
  return mq.mass(922350000) + mq.mass(922380000);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double FeedQty(double product_qty, const Assays& assays) {
  double factor =
      (assays.Product() - assays.Tails()) / (assays.Feed() - assays.Tails());
  return product_qty * factor;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double TailsQty(double product_qty, const Assays& assays) {
  double factor =
      (assays.Product() - assays.Feed()) / (assays.Feed() - assays.Tails());
  return product_qty * factor;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double ValueFunc(double frac) {
  if (frac < 0) {
    std::stringstream msg;
    msg << "The provided fraction (" << frac
        << ") is lower than the acceptable range.";
    throw ValueError(msg.str());
  }

  if (frac >= 1) {
    std::stringstream msg;
    msg << "The provided fraction (" << frac
        << ") is higher than the acceptable range.";
    throw ValueError(msg.str());
  }

  return (1 - 2 * frac) * std::log(1 / frac - 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double SwuRequired(double product_qty, const Assays& assays) {
  double feed = FeedQty(product_qty, assays);
  double tails = TailsQty(product_qty, assays);
  double swu = product_qty * ValueFunc(assays.Product()) +
               tails * ValueFunc(assays.Tails()) -
               feed * ValueFunc(assays.Feed());
  return swu;
}

}  // namespace toolkit
}  // namespace cyclus
