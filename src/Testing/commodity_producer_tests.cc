#include "commodity_producer_tests.h"

#include "error.h"

//using namespace supply_demand;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerTests::SetUp() 
{
  using cyclus::Commodity;
  using cyclus::supply_demand::CommodityProducer;
  commodity_name_ = "commodity";
  commodity_ = Commodity(commodity_name_);
  producer_ = CommodityProducer();
  capacity_ = 5.0;
  cost_ = 1.5;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerTests::TearDown() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducerTests::DefaultCapacity() 
{
  return producer_.default_capacity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducerTests::DefaultCost() 
{
  return producer_.default_cost_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerTests::AddCommodity() 
{
  producer_.AddCommodity(commodity_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerTests::SetCapacity() 
{
  producer_.SetCapacity(commodity_,capacity_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerTests::SetCost() 
{
  producer_.SetCost(commodity_,cost_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests,initialization) 
{
  EXPECT_FALSE(producer_.ProducesCommodity(commodity_));
  EXPECT_THROW(producer_.ProductionCapacity(commodity_), cyclus::KeyError);
  EXPECT_THROW(producer_.ProductionCost(commodity_), cyclus::KeyError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests,addcommodity) 
{
  EXPECT_NO_THROW(AddCommodity());
  EXPECT_EQ(producer_.ProductionCapacity(commodity_),DefaultCapacity());
  EXPECT_EQ(producer_.ProductionCost(commodity_),DefaultCost());
  EXPECT_NO_THROW(AddCommodity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests,info) 
{
  EXPECT_NO_THROW(AddCommodity());
  EXPECT_NO_THROW(SetCapacity());
  EXPECT_NO_THROW(SetCost());
  EXPECT_EQ(producer_.ProductionCapacity(commodity_),capacity_);
  EXPECT_EQ(producer_.ProductionCost(commodity_),cost_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests,copy) 
{
  using cyclus::Commodity;
  using cyclus::supply_demand::CommodityProducer;
  EXPECT_NO_THROW(AddCommodity());
  EXPECT_NO_THROW(SetCapacity());
  EXPECT_NO_THROW(SetCost());
  CommodityProducer copy;
  copy.CopyProducedCommoditiesFrom(&producer_);
  EXPECT_TRUE(copy.ProducesCommodity(commodity_));
  EXPECT_EQ(copy.ProductionCapacity(commodity_),capacity_);
  EXPECT_EQ(copy.ProductionCost(commodity_),cost_);
}
