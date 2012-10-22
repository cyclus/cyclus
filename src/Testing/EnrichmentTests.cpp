#include <gtest/gtest.h>

#include "EnrichmentTests.h"

#include "CompMap.h"
#include "CycException.h"
#include "CycLimits.h"

#include <iostream>

using namespace std;
using namespace Cyclus;
using namespace Enrichment;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void EnrichmentTests::SetUp() 
{
  feed_ = 0.0072;
  product_ = 0.05;
  tails_ = 0.002;

  assay_u_ = product_;
  mass_u_ = 10;

  CompMapPtr comp = CompMapPtr(new CompMap(ATOM));
  (*comp)[92235] = assay_u_;
  (*comp)[92238] = 1 - assay_u_;

  mat_ = mat_rsrc_ptr(new Material(comp));
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
  swu_ = mass_u_*product_val + feed_qty_*feed_val + tails_qty_*tails_val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTests,assays) 
{
  Assays a(feed_,product_,tails_);
  EXPECT_DOUBLE_EQ(feed_,a.feed());
  EXPECT_DOUBLE_EQ(product_,a.product());
  EXPECT_DOUBLE_EQ(tails_,a.tails());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTests,valuefunction) 
{
  EXPECT_THROW(value_func(0-eps()),CycRangeException);
  EXPECT_THROW(value_func(1),CycRangeException);

  double step = 0.001;
  double test_value=0;
  while (test_value < 1)
    {
      double expected = (1-2*test_value)*log(1/test_value - 1);
      EXPECT_DOUBLE_EQ(expected,value_func(test_value));
      test_value += step;
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTests,material) 
{
  EXPECT_DOUBLE_EQ(assay_u_,uranium_assay(mat_));
  EXPECT_DOUBLE_EQ(mass_u_,uranium_qty(mat_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(EnrichmentTests,enrichmentcalcs) 
{
  Assays assays(feed_,uranium_assay(mat_),tails_);
  double product_qty = uranium_qty(mat_);
  EXPECT_DOUBLE_EQ(feed_qty_,feed_qty(product_qty,assays));
  EXPECT_DOUBLE_EQ(tails_qty_,tails_qty(product_qty,assays));
  EXPECT_DOUBLE_EQ(swu_,swu_required(product_qty,assays));
}
