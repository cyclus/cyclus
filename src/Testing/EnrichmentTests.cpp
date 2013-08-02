#include <gtest/gtest.h>

#include "EnrichmentTests.h"

#include "CompMap.h"
#include "CycException.h"
#include "CycLimits.h"

#include <iostream>

//using namespace cyclus;
//using namespace enrichment;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void EnrichmentTests::SetUp() 
{
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  feed_ = 0.0072;
  product_ = 0.05;
  tails_ = 0.002;

  assay_u_ = product_;
  mass_u_ = 10;

  CompMapPtr comp = CompMapPtr(new CompMap(cyclus::ATOM));
  (*comp)[92235] = assay_u_;
  (*comp)[92238] = 1 - assay_u_;

  mat_ = cyclus::mat_rsrc_ptr(new cyclus::Material(comp));
  mat_->setQuantity(mass_u_);

  setEnrichmentParameters();
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void EnrichmentTests::TearDown() 
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void EnrichmentTests::setEnrichmentParameters()
{
  double feed_val = (1-2*feed_)*log(1/feed_ - 1);
  double tails_val = (1-2*tails_)*log(1/tails_ - 1);
  double product_val = (1-2*product_)*log(1/product_ - 1);

  feed_qty_ = mass_u_ * (product_-tails_)/(feed_-tails_);
  tails_qty_ = mass_u_ * (product_-feed_)/(feed_-tails_);
  swu_ = mass_u_*product_val + tails_qty_*tails_val - feed_qty_*feed_val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTests,assays) 
{
  cyclus::enrichment::Assays a(feed_,product_,tails_);
  EXPECT_DOUBLE_EQ(feed_,a.feed());
  EXPECT_DOUBLE_EQ(product_,a.product());
  EXPECT_DOUBLE_EQ(tails_,a.tails());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTests,valuefunction) 
{
  using cyclus::CycRangeException;
  EXPECT_THROW(cyclus::enrichment::value_func(0-cyclus::eps()),CycRangeException);
  EXPECT_THROW(cyclus::enrichment::value_func(1),CycRangeException);

  double step = 0.001;
  double test_value=0;
  while (test_value < 1)
    {
      double expected = (1-2*test_value)*log(1/test_value - 1);
      EXPECT_DOUBLE_EQ(expected, cyclus::enrichment::value_func(test_value));
      test_value += step;
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTests,material) 
{
  EXPECT_DOUBLE_EQ(assay_u_, cyclus::enrichment::uranium_assay(mat_));
  EXPECT_DOUBLE_EQ(mass_u_, cyclus::enrichment::uranium_qty(mat_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTests,enrichmentcalcs) 
{
  cyclus::enrichment::Assays assays(feed_, cyclus::enrichment::uranium_assay(mat_),
                                    tails_);
  double product_qty = cyclus::enrichment::uranium_qty(mat_);
  EXPECT_DOUBLE_EQ(feed_qty_,feed_qty(product_qty,assays));
  EXPECT_DOUBLE_EQ(tails_qty_,tails_qty(product_qty,assays));
  EXPECT_DOUBLE_EQ(swu_,swu_required(product_qty,assays));
}
