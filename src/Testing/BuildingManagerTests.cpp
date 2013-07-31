#include <gtest/gtest.h>

#include "BuildingManagerTests.h"

#include "Error.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildingManagerTests::SetUp() 
{
  using cyclus::ActionBuilding::Builder;
  demand = 1001;
  capacity1 = 800, capacity2 = 200;
  cost1 = capacity1, cost2 = capacity2;
  build1 = 1, build2 = 2;
  helper = new CommodityTestHelper();
  builder1 = new Builder();
  builder2 = new Builder();
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildingManagerTests::TearDown() 
{
  delete helper;
  delete builder1;
  delete builder2;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildingManagerTests::setUpProblem() 
{
  helper->producer1->addCommodity(helper->commodity);
  helper->producer2->addCommodity(helper->commodity);
  helper->producer1->setCapacity(helper->commodity,capacity1);
  helper->producer2->setCapacity(helper->commodity,capacity2);
  helper->producer1->setCost(helper->commodity,cost1);
  helper->producer2->setCost(helper->commodity,cost2);
  builder1->registerProducer(helper->producer1);
  builder2->registerProducer(helper->producer2);
  manager.registerBuilder(builder1);
  manager.registerBuilder(builder2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildingManagerTests,init) 
{
  EXPECT_THROW(manager.unRegisterBuilder(builder1), cyclus::KeyError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildingManagerTests,registration) 
{
  EXPECT_NO_THROW(manager.registerBuilder(builder1));
  EXPECT_THROW(manager.registerBuilder(builder1), cyclus::KeyError);
  EXPECT_NO_THROW(manager.unRegisterBuilder(builder1));
  EXPECT_THROW(manager.unRegisterBuilder(builder1), cyclus::KeyError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildingManagerTests,DISABLED_problem) 
{
  using cyclus::ActionBuilding::BuildOrder;
  setUpProblem();
  std::vector<BuildOrder> orders = manager.makeBuildDecision(helper->commodity,demand);
  EXPECT_EQ(orders.size(),2);

  BuildOrder order1 = orders.at(0);
  EXPECT_EQ(order1.number,build1);
  EXPECT_EQ(order1.builder,builder1); 
  EXPECT_EQ(order1.producer,helper->producer1);

  BuildOrder order2 = orders.at(1);
  EXPECT_EQ(order2.number,build2);
  EXPECT_EQ(order2.builder,builder2); 
  EXPECT_EQ(order2.producer,helper->producer2); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildingManagerTests,emptyorder) 
{
  using cyclus::ActionBuilding::BuildOrder;
  setUpProblem();
  std::vector<BuildOrder> orders = manager.makeBuildDecision(helper->commodity,0);
  EXPECT_TRUE(orders.empty());
}
