#include "enrichment.h"
#include "cyc_limits.h"
#include "error.h"
#include <sstream>

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enrichment::Assays::Assays(double feed, double product, double tails) :
  feed_(feed),
  product_(product),
  tails_(tails) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double enrichment::Assays::Feed() const {
  return feed_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double enrichment::Assays::Product() const {
  return product_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double enrichment::Assays::Tails() const {
  return tails_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double enrichment::UraniumAssay(Material::Ptr rsrc) {
  double value;
  double u235 = rsrc->isoVector().AtomFraction(92235);
  double u238 = rsrc->isoVector().AtomFraction(92238);

  LOG(LEV_DEBUG1, "CEnr") << "Comparing u235 atom fraction : "
                          << u235 << " with u238 atom fraction: "
                          << u238;

  if (u235 + u238 > 0) {
    value = u235 / (u235 + u238);
  } else {
    value = 0;
  }
  return value;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double enrichment::UraniumQty(Material::Ptr rsrc) {
  return rsrc->mass(92238, KG) + rsrc->mass(92235, KG);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double enrichment::FeedQty(double product_qty, const Assays& assays) {
  double factor =
    (assays.Product() - assays.Tails())
    /
    (assays.Feed() - assays.Tails());
  return product_qty * factor;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double enrichment::TailsQty(double product_qty, const Assays& assays) {
  double factor =
    (assays.Product() - assays.Feed())
    /
    (assays.Feed() - assays.Tails());
  return product_qty * factor;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double enrichment::ValueFunc(double frac) {
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

  return (1 - 2 * frac) * log(1 / frac - 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double enrichment::SwuRequired(double product_qty, const Assays& assays) {
  double feed = FeedQty(product_qty, assays);
  double tails = TailsQty(product_qty, assays);
  double swu =
    product_qty * ValueFunc(assays.Product()) +
    tails * ValueFunc(assays.Tails()) -
    feed * ValueFunc(assays.Feed());
  return swu;
}
} // namespace cyclus
