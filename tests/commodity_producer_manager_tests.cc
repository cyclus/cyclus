#include "commodity_producer_manager_tests.h"

#include "error.h"

//using namespace supply_demand;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::SetUp() 
{
  helper = new CommodityTestHelper();
  helper->SetUpProducers();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::TearDown() 
{
  delete helper;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::RegisterProducer(
  cyclus::CommodityProducer* producer) 
{
  manager.RegisterProducer(producer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManagerTests::UnRegisterProducer(
  cyclus::CommodityProducer* producer) 
{
  manager.UnRegisterProducer(producer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerManagerTests,initialization) 
{
  EXPECT_EQ(manager.TotalProductionCapacity(helper->commodity),0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerManagerTests,registerunregister)
{
  // 1 producer
  EXPECT_NO_THROW(RegisterProducer(helper->producer1));
  EXPECT_EQ(manager.TotalProductionCapacity(helper->commodity),helper->capacity);
  EXPECT_THROW(RegisterProducer(helper->producer1),cyclus::KeyError);

  // 2 producers
  EXPECT_NO_THROW(RegisterProducer(helper->producer2));
  EXPECT_EQ(manager.TotalProductionCapacity(helper->commodity),helper->nproducers*helper->capacity);
  EXPECT_THROW(RegisterProducer(helper->producer2),cyclus::KeyError);
  
  // 1 producer
  EXPECT_NO_THROW(UnRegisterProducer(helper->producer1));
  EXPECT_EQ(manager.TotalProductionCapacity(helper->commodity),helper->capacity);
  EXPECT_THROW(UnRegisterProducer(helper->producer1),cyclus::KeyError);

  // 0 producers
  EXPECT_NO_THROW(UnRegisterProducer(helper->producer2));
  EXPECT_EQ(manager.TotalProductionCapacity(helper->commodity),0.0);
  EXPECT_THROW(UnRegisterProducer(helper->producer2),cyclus::KeyError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(CommodityProducerManagerTests,differentcommodity)
{
  EXPECT_NO_THROW(RegisterProducer(helper->producer1));
  
  cyclus::Commodity differentcommodity("differentcommodity");
  EXPECT_EQ(manager.TotalProductionCapacity(differentcommodity),0.0);
}
