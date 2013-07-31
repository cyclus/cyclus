#include <gtest/gtest.h>

#include "SDManagerTests.h"

#include "Error.h"

//using namespace SupplyDemand;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SDManagerTests::SetUp()
{
  helper = new CommodityTestHelper();
  helper->setUpProducerManager();
  demand = cyclus::FunctionPtr(new cyclus::LinearFunction(100,100));
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SDManagerTests::TearDown() 
{
  delete helper;
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests,initialization) {
  EXPECT_FALSE(manager.managesCommodity(helper->commodity));
  EXPECT_THROW(manager.supply(helper->commodity), cyclus::KeyError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests,registercommodity) {
  EXPECT_NO_THROW(manager.registerCommodity(helper->commodity,demand));
  EXPECT_TRUE(manager.managesCommodity(helper->commodity));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests,supply) {
  EXPECT_NO_THROW(manager.registerCommodity(helper->commodity,demand));
  EXPECT_NO_THROW(manager.registerProducerManager(&helper->manager));
  EXPECT_EQ(manager.supply(helper->commodity),helper->nproducers*helper->capacity);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests,demand) {
  EXPECT_NO_THROW(manager.registerCommodity(helper->commodity,demand));
  EXPECT_EQ(manager.demandFunction(helper->commodity),demand);
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(manager.demand(helper->commodity,i),demand->value(i));
  }
}
