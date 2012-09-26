#include "CommodityProducerManagerTests.h"

#include "CycException.h"

using namespace std;
using namespace SupplyDemand;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::SetUp() 
{
  commodity_name_ = "commodity";
  commodity_ = Commodity(commodity_name_);
  manager_ = CommodityProducerManager();
  producer1_ = new CommodityProducer();
  producer2_ = new CommodityProducer();
  nproducers_ = 2;
  capacity_ = 5.0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::TearDown() 
{
  delete producer1_;
  delete producer2_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::addCommodity(SupplyDemand::CommodityProducer* producer) 
{
  producer->addCommodity(commodity_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::setCapacity(SupplyDemand::CommodityProducer* producer) 
{
  producer->setCapacity(commodity_,capacity_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::setUpProducers()
{
  addCommodity(producer1_);
  setCapacity(producer1_);
  addCommodity(producer2_);
  setCapacity(producer2_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::registerProducer(SupplyDemand::CommodityProducer* producer) 
{
  manager_.registerProducer(producer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::unRegisterProducer(SupplyDemand::CommodityProducer* producer) 
{
  manager_.unRegisterProducer(producer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerManagerTests,initialization) 
{
  EXPECT_EQ(manager_.totalProductionCapacity(commodity_),0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerManagerTests,registerunregister)
{
  setUpProducers();
  // 1 producer
  EXPECT_NO_THROW(registerProducer(producer1_));
  EXPECT_EQ(manager_.totalProductionCapacity(commodity_),capacity_);
  EXPECT_THROW(registerProducer(producer1_),CycDoubleRegistrationException);

  // 2 producers
  EXPECT_NO_THROW(registerProducer(producer2_));
  EXPECT_EQ(manager_.totalProductionCapacity(commodity_),nproducers_*capacity_);
  EXPECT_THROW(registerProducer(producer2_),CycDoubleRegistrationException);
  
  // 1 producer
  EXPECT_NO_THROW(unRegisterProducer(producer1_));
  EXPECT_EQ(manager_.totalProductionCapacity(commodity_),capacity_);
  EXPECT_THROW(unRegisterProducer(producer1_),CycNotRegisteredException);

  // 0 producers
  EXPECT_NO_THROW(unRegisterProducer(producer2_));
  EXPECT_EQ(manager_.totalProductionCapacity(commodity_),0.0);
  EXPECT_THROW(unRegisterProducer(producer2_),CycNotRegisteredException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerManagerTests,differentcommodity)
{
  setUpProducers();
  EXPECT_NO_THROW(registerProducer(producer1_));
  
  Commodity different_commodity("different_commodity");
  EXPECT_EQ(manager_.totalProductionCapacity(different_commodity),0.0);
}
