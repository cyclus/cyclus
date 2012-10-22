#include "Enrichment.h"
#include "CycLimits.h"
#include <sstream>

using namespace std;
using namespace cyclus;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
enrichment::Assays::Assays(double feed, double product, double tails) :
  feed_(feed),
  product_(product),
  tails_(tails)
{}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double enrichment::Assays::feed() const
{
  return feed_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double enrichment::Assays::product() const
{
  return product_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double enrichment::Assays::tails() const
{
  return tails_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double enrichment::uranium_assay(mat_rsrc_ptr rsrc)
{
  double value;
  double u235 = rsrc->isoVector().atomFraction(92235);
  double u238 = rsrc->isoVector().atomFraction(92238);
  if (u235 + u238 > 0)
    {
      value = u235 / (u235 + u238);
    }
  else
    {
      value = 0;
    }
  return value;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double enrichment::uranium_qty(mat_rsrc_ptr rsrc)
{
  return rsrc->mass(92238,KG) + rsrc->mass(92235,KG);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double enrichment::feed_qty(double product_qty, const Assays& assays) 
{
  double factor = 
    (assays.product() - assays.tails())
    /
    (assays.feed() - assays.tails());
  return product_qty * factor;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double enrichment::tails_qty(double product_qty, const Assays& assays) 
{
  double factor = 
    (assays.product() - assays.feed())
    /
    (assays.feed() - assays.tails());
  return product_qty * factor;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double enrichment::value_func(double frac) 
{
  if (frac < 0)
    {
      stringstream msg;
      msg << "The provided fraction (" << frac 
          << ") is lower than the acceptable range.";
      throw CycRangeException(msg.str());
    }

  if (frac >= 1)
    {
      stringstream msg;
      msg << "The provided fraction (" << frac 
          << ") is higher than the acceptable range.";
      throw CycRangeException(msg.str());
    }

  return (1-2*frac)*log(1/frac - 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double enrichment::swu_required(double product_qty, const Assays& assays) 
{
  double feed = feed_qty(product_qty,assays);
  double tails = tails_qty(product_qty,assays);
  double swu = 
    product_qty * value_func(assays.product()) +
    feed * value_func(assays.feed()) +
    tails * value_func(assays.tails());
  return swu;
}
