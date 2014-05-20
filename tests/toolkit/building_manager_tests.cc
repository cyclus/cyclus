#include "building_manager_tests.h"

#include <gtest/gtest.h>

#include "error.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BuildingManagerTests::SetUp() {
  demand = 1001;
  capacity1 = 800, capacity2 = 200;
  cost1 = capacity1, cost2 = capacity2;
  build1 = 1, build2 = 2;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BuildingManagerTests::TearDown() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BuildingManagerTests::SetUpProblem() {
  helper.producer1->AddCommodity(helper.commodity);
  helper.producer2->AddCommodity(helper.commodity);
  helper.producer1->SetCapacity(helper.commodity, capacity1);
  helper.producer2->SetCapacity(helper.commodity, capacity2);
  helper.producer1->SetCost(helper.commodity, cost1);
  helper.producer2->SetCost(helper.commodity, cost2);
  builder1.RegisterProducer(helper.producer1);
  builder2.RegisterProducer(helper.producer2);
  manager.RegisterBuilder(&builder1);
  manager.RegisterBuilder(&builder2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildingManagerTests, init) {
  EXPECT_THROW(manager.UnRegisterBuilder(&builder1), KeyError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildingManagerTests, registration) {
  EXPECT_NO_THROW(manager.RegisterBuilder(&builder1));
  EXPECT_THROW(manager.RegisterBuilder(&builder1), KeyError);
  EXPECT_NO_THROW(manager.UnRegisterBuilder(&builder1));
  EXPECT_THROW(manager.UnRegisterBuilder(&builder1), KeyError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildingManagerTests, problem) {
  SetUpProblem();
  EXPECT_EQ(manager.builders().count(&builder1), 1);
  EXPECT_EQ(manager.builders().count(&builder2), 1);

  std::vector<BuildOrder> orders = manager.MakeBuildDecision(helper.commodity,
                                                             demand);
  EXPECT_EQ(orders.size(), 2);

  BuildOrder order1 = orders.at(0);
  EXPECT_EQ(order1.number, build1);
  EXPECT_EQ(order1.builder, &builder1);
  EXPECT_EQ(order1.producer, helper.producer1);

  BuildOrder order2 = orders.at(1);
  EXPECT_EQ(order2.number, build2);
  EXPECT_EQ(order2.builder, &builder2);
  EXPECT_EQ(order2.producer, helper.producer2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildingManagerTests, emptyorder) {
  SetUpProblem();
  std::vector<BuildOrder> orders = manager.MakeBuildDecision(helper.commodity,
                                                             0);
  EXPECT_TRUE(orders.empty());
}

} // namespace toolkit
} // namespace cyclus
