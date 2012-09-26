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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerTests::TearDown() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducerTests::defaultCapacity() 
{
  return producer_.default_capacity_;
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
TEST_F(CommodityProducerTests,initialization) 
{
  EXPECT_FALSE(producer_.producesCommodity(commodity_));
  EXPECT_THROW(producer_.productionCapacity(commodity_),CycNotRegisteredException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests,addcommodity) 
{
  EXPECT_NO_THROW(addCommodity());
  EXPECT_EQ(producer_.productionCapacity(commodity_),defaultCapacity());
  EXPECT_THROW(addCommodity(),CycDoubleRegistrationException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerTests,setcapacity) 
{
  EXPECT_NO_THROW(addCommodity());
  EXPECT_NO_THROW(setCapacity());
  EXPECT_EQ(producer_.productionCapacity(commodity_),capacity_);
}
