// building_manager_tests.h
#include <gtest/gtest.h>

#include "building_manager.h"
#include "commodity_test_helper.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class BuildingManagerTests : public ::testing::Test {
 protected:
  CommodityTestHelper* helper;
  cyclus::action_building::BuildingManager manager;
  cyclus::action_building::Builder* builder1;
  cyclus::action_building::Builder* builder2;
  double demand, capacity1, capacity2, cost1, cost2;
  int build1, build2;

 public:

  virtual void SetUp();  
  virtual void TearDown();
  void SetUpProblem();
};
