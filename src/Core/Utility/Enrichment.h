// Enrichment.h
#ifndef _ENRICHMENT_H
#define _ENRICHMENT_H

#include "Material.h"

namespace Enrichment
{
  /**
     a simple container class for enrichment assays
   */
  class Assays
  {
  public:
    /// constructor
    Assays(double feed, double product, double tails);
    
    /// returns the feed assay
    double feed() const;

    /// returns the product assay
    double product() const;

    /// returns the tails assay
    double tails() const;

  private:
    double feed_, product_, tails_;
  };

  /**
     @param mat the material inquired about
     @return the atom percent of U-235 w.r.t Uranium in a material
   */
  double uranium_assay(mat_rsrc_ptr mat);

  /**
     @param mat the material inquired about
     @return the quantity of uranium in a material
   */
  double uranium_qty(mat_rsrc_ptr mat);

  /**
     @param product_qty the amount of product Uranium
     @param assays the assay of product, feed, and tails
     @return the quantity of feedstock required to make the product
   */
  double feed_qty(double product_qty, const Assays& assays);

  /**
     @param product_qty the amount of product Uranium
     @param assays the assay of product, feed, and tails
     @return the quantity of tails resulting from enriching the product
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
     value is not between 0 and 1
     @return the value function for a given fraction between 0 and 1
   */
  double value_func(double frac);
};

#endif
