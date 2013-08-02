// BuildingManagerTests.h
#include <gtest/gtest.h>

#include "BuildingManager.h"
#include "CommodityTestHelper.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class BuildingManagerTests : public ::testing::Test {
 protected:
  CommodityTestHelper* helper;
  cyclus::ActionBuilding::BuildingManager manager;
  cyclus::ActionBuilding::Builder* builder1;
  cyclus::ActionBuilding::Builder* builder2;
  double demand, capacity1, capacity2, cost1, cost2;
  int build1, build2;

 public:

  virtual void SetUp();  
  virtual void TearDown();
  void setUpProblem();
};
