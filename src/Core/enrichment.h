// enrichment.h
#ifndef _ENRICHMENT_H
#define _ENRICHMENT_H

#include <set>
#include "material.h"

namespace cyclus {
namespace enrichment {
  /**
     a simple container class for enrichment assays
   */
  class Assays {
  public:
    /// constructor
    Assays(double feed, double product, double tails);
    
    /**
       @return the feed assay as an atomic fraction, i.e. for
       0.711% U-235 in natural uranium, this will return 0.00711.
    */
    double Feed() const;

    /**
       @return the product assay as an atomic fraction, i.e. for
       3% U-235 in enriched uranium, this will return 0.03.
    */
    double Product() const;

    /**
       @return the tails assay as an atomic fraction, i.e. for
       0.2% U-235 in depleted uranium, this will return 0.002.
    */
    double Tails() const;

  private:
    double feed_, product_, tails_;
  };

  /**
     @param mat the material inquired about
     @return the atom percent of U-235 w.r.t Uranium in a material
   */
  double uranium_assay(Material::Ptr mat);

  /**
     @param mat the material inquired about
     @return the quantity of uranium in a material whose units match
     those of the given material
   */
  double uranium_qty(Material::Ptr mat);

  /**
     @param product_qty the amount of product Uranium
     @param assays the assay of product, feed, and tails
     @return the quantity of feedstock required to make the product 
     whose units match those of the given product
   */
  double feed_qty(double product_qty, const Assays& assays);

  /**
     @param product_qty the amount of product Uranium
     @param assays the assay of product, feed, and tails
     @return the quantity of tails resulting from enriching the product
     whose units match those of the given product
   */
  double tails_qty(double product_qty, const Assays& assays);

  /**
     @param product_qty the amount of product Uranium
     @param assays the assay of product, feed, and tails
     @return the amount of swu required to enrich the product
   */
  double swu_required(double product_qty, const Assays& assays);

  /**
     @param frac the fraction input, this will throw if the fraction
     value is not in [0,1)
     @return the value function for a given fraction in [0,1)
   */
  double value_func(double frac);
};
} // namespace cyclus
#endif
