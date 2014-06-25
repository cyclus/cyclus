#include "commodity_producer_tests.h"

#include "error.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityProducerTests::SetUp() {
  commodity_name_ = "commodity";
  commodity_ = Commodity(commodity_name_);
  producer_ = CommodityProducer();
  capacity_ = 5.0;
  cost_ = 1.5;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityProducerTests::TearDown() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests, initialization) {
  EXPECT_FALSE(producer_.Produces(commodity_));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests, addcommodity) {
  producer_.Add(commodity_);
  EXPECT_TRUE(producer_.Produces(commodity_));
  EXPECT_EQ(producer_.Capacity(commodity_), 0);
  EXPECT_EQ(producer_.Cost(commodity_), kModifierLimit);
  producer_.SetCapacity(commodity_, capacity_);
  producer_.SetCost(commodity_, cost_);
  EXPECT_EQ(producer_.Capacity(commodity_), capacity_);
  EXPECT_EQ(producer_.Cost(commodity_), cost_);
  producer_.Rm(commodity_);
  EXPECT_FALSE(producer_.Produces(commodity_));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests, copy) {
  producer_.Add(commodity_);
  producer_.SetCapacity(commodity_, capacity_);
  producer_.SetCost(commodity_, cost_);
  CommodityProducer copy;
  copy.Copy(&producer_);
  EXPECT_TRUE(copy.Produces(commodity_));
  EXPECT_EQ(copy.Capacity(commodity_), capacity_);
  EXPECT_EQ(copy.Cost(commodity_), cost_);
}

}  // namespace toolkit
}  // namespace cyclus
