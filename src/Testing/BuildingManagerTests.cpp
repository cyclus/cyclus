#include <gtest/gtest.h>

#include "BuildingManagerTests.h"

#include "SupplyDemand.h"
#include "SupplyDemandManager.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildingManagerTests::SetUp() {
  sdmanager_ = new SupplyDemandManager();
  initSDManager();
  manager_ = new BuildingManager(sdmanager_);
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildingManagerTests::TearDown() {
  delete sdmanager_, commod_, manager_, p1_, p2_;
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildingManagerTests::initSDManager() {
  commod_ = new Commodity("commod_");
  demand_ = 1001;

  capacity1_ = 800;
  cost1_ = capacity1_;
  p1_ = new Producer("p1",*commod_,capacity1_,cost1_);

  capacity2_ = 200;
  cost2_ = capacity2_;
  p2_ = new Producer("p2",*commod_,capacity2_,cost2_);

  vector<Producer> v;
  v.push_back(*p1_);
  v.push_back(*p2_);

  FunctionPtr demand = FunctionPtr(new LinearFunction(5,5));

  sdmanager_->registerCommodity(*commod_,demand,v);
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildingManagerTests,TestEmptyOrder) {
  vector<BuildOrder> v = manager_->makeBuildDecision(*commod_,0);
  EXPECT_TRUE(v.empty());
  v = manager_->makeBuildDecision(*commod_,-5);
  EXPECT_TRUE(v.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(BuildingManagerTests,TestBuildDecision) {
  vector<BuildOrder> orders = 
    manager_->makeBuildDecision(*commod_,demand_);
  EXPECT_EQ(orders.size(),2);
  EXPECT_EQ(orders.at(0).number,1);
  EXPECT_EQ(*orders.at(0).producer,*p1_);
  EXPECT_EQ(orders.at(1).number,2);
  EXPECT_EQ(*orders.at(1).producer,*p2_);
}
