#include "sd_manager_tests.h"

#include <gtest/gtest.h>

#include "error.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SDManagerTests::SetUp() {
  helper = new CommodityTestHelper();
  helper->SetUpProducerManager();
  demand = cyclus::SymFunction::Ptr(new cyclus::LinearFunction(100, 100));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SDManagerTests::TearDown() {
  delete helper;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests, initialization) {
  EXPECT_FALSE(manager.ManagesCommodity(helper->commodity));
  EXPECT_THROW(manager.Supply(helper->commodity), cyclus::KeyError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests, registercommodity) {
  EXPECT_NO_THROW(manager.RegisterCommodity(helper->commodity, demand));
  EXPECT_TRUE(manager.ManagesCommodity(helper->commodity));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests, supply) {
  EXPECT_NO_THROW(manager.RegisterCommodity(helper->commodity, demand));
  EXPECT_NO_THROW(manager.RegisterProducerManager(&helper->manager));
  EXPECT_EQ(manager.Supply(helper->commodity),
            helper->nproducers*helper->capacity);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests, demand) {
  EXPECT_NO_THROW(manager.RegisterCommodity(helper->commodity, demand));
  EXPECT_EQ(manager.DemandFunction(helper->commodity), demand);
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(manager.Demand(helper->commodity, i), demand->value(i));
  }
}
