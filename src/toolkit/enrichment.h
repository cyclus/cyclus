#ifndef CYCLUS_SRC_TOOLKIT_ENRICHMENT_H_
#define CYCLUS_SRC_TOOLKIT_ENRICHMENT_H_

#include <set>

#include "material.h"

namespace cyclus {
namespace toolkit {

/// A simple container class for enrichment assays
class Assays {
 public:
  /// Constructor
  Assays(double feed, double product, double tails);

  /// @return the feed assay as an atomic fraction, i.e. for
  /// 0.711% U-235 in natural uranium, this will return 0.00711.
  double Feed() const;

  /// @return the product assay as an atomic fraction, i.e. for
  /// 3% U-235 in enriched uranium, this will return 0.03.
  double Product() const;

  /// @return the tails assay as an atomic fraction, i.e. for
  /// 0.2% U-235 in depleted uranium, this will return 0.002.
  double Tails() const;

 private:
  double feed_, product_, tails_;
};

/// @param mat the material inquired about
/// @return the atom percent of U-235 w.r.t Uranium in a material
double UraniumAssay(Material::Ptr mat);

/// inline double UraniumAssay(Material::Ptr mat) {
///   return UraniumAssay(mat.get());
/// }

/// @param mat the material inquired about
/// @return the quantity of uranium in a material whose units match
/// those of the given material
double UraniumQty(Material::Ptr mat);

/// inline double UraniumQty(Material::Ptr mat) { return UraniumQty(mat.get()); }

/// @param product_qty the amount of product Uranium
/// @param assays the assay of product, feed, and tails
/// @return the quantity of feedstock required to make the product
/// whose units match those of the given product
double FeedQty(double product_qty, const Assays& assays);

/// @param product_qty the amount of product Uranium
/// @param assays the assay of product, feed, and tails
/// @return the quantity of tails resulting from enriching the product
/// whose units match those of the given product
double TailsQty(double product_qty, const Assays& assays);

/// @param product_qty the amount of product Uranium
/// @param assays the assay of product, feed, and tails
/// @return the amount of swu required to enrich the product
double SwuRequired(double product_qty, const Assays& assays);

/// @param frac the fraction input, this will throw if the fraction
/// value is not in [0,1)
/// @return the value function for a given fraction in [0,1)
double ValueFunc(double frac);

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_ENRICHMENT_H_
