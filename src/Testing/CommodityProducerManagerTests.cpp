#include "CommodityProducerManagerTests.h"

#include "CycException.h"

//using namespace SupplyDemand;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::SetUp() 
{
  helper = new CommodityTestHelper();
  helper->setUpProducers();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::TearDown() 
{
  delete helper;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::registerProducer(
  cyclus::SupplyDemand::CommodityProducer* producer) 
{
  manager.registerProducer(producer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::unRegisterProducer(
  cyclus::SupplyDemand::CommodityProducer* producer) 
{
  manager.unRegisterProducer(producer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerManagerTests,initialization) 
{
  EXPECT_EQ(manager.totalProductionCapacity(helper->commodity),0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerManagerTests,registerunregister)
{
  using cyclus::CycDoubleRegistrationException;
  using cyclus::CycNotRegisteredException;
  // 1 producer
  EXPECT_NO_THROW(registerProducer(helper->producer1));
  EXPECT_EQ(manager.totalProductionCapacity(helper->commodity),helper->capacity);
  EXPECT_THROW(registerProducer(helper->producer1),CycDoubleRegistrationException);

  // 2 producers
  EXPECT_NO_THROW(registerProducer(helper->producer2));
  EXPECT_EQ(manager.totalProductionCapacity(helper->commodity),helper->nproducers*helper->capacity);
  EXPECT_THROW(registerProducer(helper->producer2),CycDoubleRegistrationException);
  
  // 1 producer
  EXPECT_NO_THROW(unRegisterProducer(helper->producer1));
  EXPECT_EQ(manager.totalProductionCapacity(helper->commodity),helper->capacity);
  EXPECT_THROW(unRegisterProducer(helper->producer1),CycNotRegisteredException);

  // 0 producers
  EXPECT_NO_THROW(unRegisterProducer(helper->producer2));
  EXPECT_EQ(manager.totalProductionCapacity(helper->commodity),0.0);
  EXPECT_THROW(unRegisterProducer(helper->producer2),CycNotRegisteredException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerManagerTests,differentcommodity)
{
  EXPECT_NO_THROW(registerProducer(helper->producer1));
  
  cyclus::Commodity differentcommodity("differentcommodity");
  EXPECT_EQ(manager.totalProductionCapacity(differentcommodity),0.0);
}
