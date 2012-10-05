#include "CommodityProducerTests.h"

#include "CycException.h"

using namespace std;
using namespace SupplyDemand;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerTests::SetUp() 
{
  commodity_name_ = "commodity";
  commodity_ = Commodity(commodity_name_);
  producer_ = CommodityProducer();
  capacity_ = 5.0;
  cost_ = 1.5;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerTests::TearDown() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducerTests::defaultCapacity() 
{
  return producer_.default_capacity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducerTests::defaultCost() 
{
  return producer_.default_cost_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerTests::addCommodity() 
{
  producer_.addCommodity(commodity_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerTests::setCapacity() 
{
  producer_.setCapacity(commodity_,capacity_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerTests::setCost() 
{
  producer_.setCost(commodity_,cost_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests,initialization) 
{
  EXPECT_FALSE(producer_.producesCommodity(commodity_));
  EXPECT_THROW(producer_.productionCapacity(commodity_),CycNotRegisteredException);
  EXPECT_THROW(producer_.productionCost(commodity_),CycNotRegisteredException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests,addcommodity) 
{
  EXPECT_NO_THROW(addCommodity());
  EXPECT_EQ(producer_.productionCapacity(commodity_),defaultCapacity());
  EXPECT_EQ(producer_.productionCost(commodity_),defaultCost());
  EXPECT_THROW(addCommodity(),CycDoubleRegistrationException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests,info) 
{
  EXPECT_NO_THROW(addCommodity());
  EXPECT_NO_THROW(setCapacity());
  EXPECT_NO_THROW(setCost());
  EXPECT_EQ(producer_.productionCapacity(commodity_),capacity_);
  EXPECT_EQ(producer_.productionCost(commodity_),cost_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests,copy) 
{
  EXPECT_NO_THROW(addCommodity());
  EXPECT_NO_THROW(setCapacity());
  EXPECT_NO_THROW(setCost());
  CommodityProducer copy;
  copy.copyProducedCommoditiesFrom(&producer_);
  EXPECT_TRUE(copy.producesCommodity(commodity_));
  EXPECT_EQ(copy.productionCapacity(commodity_),capacity_);
  EXPECT_EQ(copy.productionCost(commodity_),cost_);
}
