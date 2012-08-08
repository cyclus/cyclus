#include <gtest/gtest.h>

#include "SDManagerTests.h"

#include "CycException.h"
#include "SupplyDemandManager.h"
#include "SupplyDemand.h"
#include "SymbolicFunctions.h"

#include <vector>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SDManagerTests::SetUp() {
  manager = SupplyDemandManager();
  
  commod = new Commodity("commod");
  demand = FunctionPtr(new LinearFunction(5,5));

  capacity = 1;
  cost = 1;
  p1 = new Producer("p1",*commod,capacity,cost);
  p2 = new Producer("p2",*commod,capacity,cost);

  initCommodity();
  initMarketPlayers();
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SDManagerTests::TearDown() {
  delete p1;
  delete p2;
  player1_->leaveMarket(*commod);
  player2_->leaveMarket(*commod);
  delete mpmanager_;
  delete player1_;
  delete player2_;
  delete commod;
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SDManagerTests::initCommodity() {
  vector<Producer> v;
  v.push_back(*p1);
  v.push_back(*p2);
  manager.registerCommodity(*commod,demand,v);
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SDManagerTests::initMarketPlayers() {
  cap1_ = 15;
  cap2_ = 20;

  mpmanager_ = new MarketPlayerManager();
  mpmanager_->setCommodity(*commod);

  player1_ = new MarketPlayer();
  player1_->registerCommodity(*commod);
  player1_->setProductionCapacity(cap1_,*commod);
  player1_->registerManager(mpmanager_,*commod);
  player1_->enterMarket(*commod);

  player2_ = new MarketPlayer();
  player2_->registerCommodity(*commod);
  player2_->setProductionCapacity(cap2_,*commod);
  player2_->registerManager(mpmanager_,*commod);
  player2_->enterMarket(*commod);

  manager.registerPlayerManager(*commod,mpmanager_);
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests,TestProduceres) {
  EXPECT_EQ(manager.nProducers(*commod),2);
  EXPECT_EQ(*manager.producer(*commod,0),*p1);
  EXPECT_EQ(*manager.producer(*commod,1),*p2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests,TestDemand) {
  EXPECT_EQ(manager.demandFunction(*commod),demand);
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(manager.demand(*commod,i),demand->value(i));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SDManagerTests,TestSupply) {
  EXPECT_EQ(manager.supply(*commod),cap1_+cap2_);
}
