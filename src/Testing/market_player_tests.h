// market_player_tests.h
#include <gtest/gtest.h>

#include "market_player.h"
#include "market_player_manager.h"

#include "supply_demand.h"

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
  void InitPlayers();
};
