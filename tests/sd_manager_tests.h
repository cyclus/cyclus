#ifndef CYCLUS_TESTS_SD_MANAGER_TESTS_H_
#define CYCLUS_TESTS_SD_MANAGER_TESTS_H_

// sd_manager_tests.h
#include <gtest/gtest.h>

#include "commodity_test_helper.h"
#include "supply_demand_manager.h"
#include "symbolic_functions.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SDManagerTests : public ::testing::Test {
 public:
  cyclus::SupplyDemandManager manager;
  cyclus::SymFunction::Ptr demand;
  CommodityTestHelper* helper;

  virtual void SetUp();
  virtual void TearDown();
};

#endif  // CYCLUS_TESTS_SD_MANAGER_TESTS_H_
