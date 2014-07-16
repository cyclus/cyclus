#ifndef CYCLUS_TESTS_TOOLKIT_SD_MANAGER_TESTS_H_
#define CYCLUS_TESTS_TOOLKIT_SD_MANAGER_TESTS_H_

#include <gtest/gtest.h>

#include "toolkit/commodity_test_helper.h"
#include "toolkit/supply_demand_manager.h"
#include "toolkit/symbolic_functions.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SDManagerTests : public ::testing::Test {
 public:
  SupplyDemandManager manager;
  SymFunction::Ptr demand;
  CommodityTestHelper* helper;

  virtual void SetUp();
  virtual void TearDown();
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_TESTS_TOOLKIT_SD_MANAGER_TESTS_H_
