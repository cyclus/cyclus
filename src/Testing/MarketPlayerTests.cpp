#include <gtest/gtest.h>

#include "MarketPlayerTests.h"

#include "error.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void MarketPlayerTests::SetUp() {
  using cyclus::Commodity;
  using cyclus::MarketPlayer;
  using cyclus::MarketPlayerManager;
  commodA_ = new Commodity("A");
  commodB_ = new Commodity("B");
  commodC_ = new Commodity("C");
  managerA_ = new MarketPlayerManager();
  managerA_->SetCommodity(*commodA_);
  managerB_ = new MarketPlayerManager();
  managerB_->SetCommodity(*commodB_);
  player1_ = new MarketPlayer();
  player2_ = new MarketPlayer();
  nPlayers_ = 2;
  capacityA1_ = 4.5;
  capacityB1_ = 5.1;
  capacityA2_ = 6.7;
  capacityB2_ = 7.9;
  InitPlayers();
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
void MarketPlayerTests::InitPlayers() {
  player1_->RegisterCommodity(*commodA_);
  player1_->SetProductionCapacity(capacityA1_,*commodA_);
  player1_->RegisterManager(managerA_,*commodA_);
  player2_->RegisterCommodity(*commodA_);
  player2_->SetProductionCapacity(capacityA2_,*commodA_);
  player2_->RegisterManager(managerA_,*commodA_);
  player1_->RegisterCommodity(*commodB_);
  player1_->SetProductionCapacity(capacityB1_,*commodB_);
  player1_->RegisterManager(managerB_,*commodB_);
  player2_->RegisterCommodity(*commodB_);
  player2_->SetProductionCapacity(capacityB2_,*commodB_);
  player2_->RegisterManager(managerB_,*commodB_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MarketPlayerTests,EnterLeaveTests) {
  // empty
  EXPECT_EQ(managerA_->PlayerProductionCapacity(),0);
  EXPECT_EQ(managerB_->PlayerProductionCapacity(),0);
  // player 1 enters
  player1_->EnterMarket(*commodA_);
  player1_->EnterMarket(*commodB_);
  EXPECT_EQ(managerA_->PlayerProductionCapacity(),capacityA1_);
  EXPECT_EQ(managerB_->PlayerProductionCapacity(),capacityB1_);
  // player 2 enters
  player2_->EnterMarket(*commodA_);
  player2_->EnterMarket(*commodB_);
  EXPECT_EQ(managerA_->PlayerProductionCapacity(),
            capacityA1_+capacityA2_);
  EXPECT_EQ(managerB_->PlayerProductionCapacity(),
            capacityB1_+capacityB2_);
  // player 1 leaves
  player1_->LeaveMarket(*commodA_);
  player1_->LeaveMarket(*commodB_);
  EXPECT_EQ(managerA_->PlayerProductionCapacity(),capacityA2_);
  EXPECT_EQ(managerB_->PlayerProductionCapacity(),capacityB2_);
  // player 2 leaves
  player2_->LeaveMarket(*commodA_);
  player2_->LeaveMarket(*commodB_);
  EXPECT_EQ(managerA_->PlayerProductionCapacity(),0);
  EXPECT_EQ(managerB_->PlayerProductionCapacity(),0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MarketPlayerTests,ExceptionTests) {
  EXPECT_THROW(player1_->ProductionCapacity(*commodC_), cyclus::KeyError);
  EXPECT_THROW(player1_->RegisterManager(managerB_,*commodC_),
               cyclus::KeyError);
  EXPECT_THROW(player1_->RegisterManager(managerA_,*commodB_),
               cyclus::Error);
}
