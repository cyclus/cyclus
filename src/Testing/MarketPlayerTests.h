// MarketPlayerTests.h
#include <gtest/gtest.h>

#include "MarketPlayer.h"
#include "MarketPlayerManager.h"

#include "SupplyDemand.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MarketPlayerTests : public ::testing::Test {
 protected:
  MarketPlayerManager* managerA_;
  MarketPlayerManager* managerB_;
  MarketPlayer* player1_;
  MarketPlayer* player2_;
  Commodity* commodA_;
  Commodity* commodB_;
  Commodity* commodC_;
  int nPlayers_;
  double capacityA1_;
  double capacityB1_;
  double capacityA2_;
  double capacityB2_;

 public:
  virtual void SetUp();
  virtual void TearDown();
  void initPlayers();
};
