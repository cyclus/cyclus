// MarketPlayerTests.h
#include <gtest/gtest.h>

#include "MarketPlayer.h"
#include "MarketPlayerManager.h"

#include "SupplyDemand.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MarketPlayerTests : public ::testing::Test {
 protected:
  cyclus::MarketPlayerManager* managerA_;
  cyclus::MarketPlayerManager* managerB_;
  cyclus::MarketPlayer* player1_;
  cyclus::MarketPlayer* player2_;
  cyclus::Commodity* commodA_;
  cyclus::Commodity* commodB_;
  cyclus::Commodity* commodC_;
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
