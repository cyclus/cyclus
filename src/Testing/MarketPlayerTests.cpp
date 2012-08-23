#include <gtest/gtest.h>

#include "MarketPlayerTests.h"

#include "CycException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void MarketPlayerTests::SetUp() {
  commodA_ = new Commodity("A");
  commodB_ = new Commodity("B");
  commodC_ = new Commodity("C");
  managerA_ = new MarketPlayerManager();
  managerA_->setCommodity(*commodA_);
  managerB_ = new MarketPlayerManager();
  managerB_->setCommodity(*commodB_);
  player1_ = new MarketPlayer();
  player2_ = new MarketPlayer();
  nPlayers_ = 2;
  capacityA1_ = 4.5;
  capacityB1_ = 5.1;
  capacityA2_ = 6.7;
  capacityB2_ = 7.9;
  initPlayers();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void MarketPlayerTests::TearDown() {
  delete managerA_;
  delete managerB_;
  delete player1_;
  delete player2_;
  delete commodA_;
  delete commodB_;
  delete commodC_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void MarketPlayerTests::initPlayers() {
  player1_->registerCommodity(*commodA_);
  player1_->setProductionCapacity(capacityA1_,*commodA_);
  player1_->registerManager(managerA_,*commodA_);
  player2_->registerCommodity(*commodA_);
  player2_->setProductionCapacity(capacityA2_,*commodA_);
  player2_->registerManager(managerA_,*commodA_);
  player1_->registerCommodity(*commodB_);
  player1_->setProductionCapacity(capacityB1_,*commodB_);
  player1_->registerManager(managerB_,*commodB_);
  player2_->registerCommodity(*commodB_);
  player2_->setProductionCapacity(capacityB2_,*commodB_);
  player2_->registerManager(managerB_,*commodB_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MarketPlayerTests,EnterLeaveTests) {
  // empty
  EXPECT_EQ(managerA_->playerProductionCapacity(),0);
  EXPECT_EQ(managerB_->playerProductionCapacity(),0);
  // player 1 enters
  player1_->enterMarket(*commodA_);
  player1_->enterMarket(*commodB_);
  EXPECT_EQ(managerA_->playerProductionCapacity(),capacityA1_);
  EXPECT_EQ(managerB_->playerProductionCapacity(),capacityB1_);
  // player 2 enters
  player2_->enterMarket(*commodA_);
  player2_->enterMarket(*commodB_);
  EXPECT_EQ(managerA_->playerProductionCapacity(),
            capacityA1_+capacityA2_);
  EXPECT_EQ(managerB_->playerProductionCapacity(),
            capacityB1_+capacityB2_);
  // player 1 leaves
  player1_->leaveMarket(*commodA_);
  player1_->leaveMarket(*commodB_);
  EXPECT_EQ(managerA_->playerProductionCapacity(),capacityA2_);
  EXPECT_EQ(managerB_->playerProductionCapacity(),capacityB2_);
  // player 2 leaves
  player2_->leaveMarket(*commodA_);
  player2_->leaveMarket(*commodB_);
  EXPECT_EQ(managerA_->playerProductionCapacity(),0);
  EXPECT_EQ(managerB_->playerProductionCapacity(),0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MarketPlayerTests,ExceptionTests) {
  EXPECT_THROW(player1_->productionCapacity(*commodC_),CycKeyException);
  EXPECT_THROW(player1_->registerManager(managerB_,*commodC_),
               CycKeyException);
  EXPECT_THROW(player1_->registerManager(managerA_,*commodB_),
               CycCommodityMismatchError);
}
