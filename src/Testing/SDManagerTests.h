// SDManagerTests.h
#include <gtest/gtest.h>

#include "SupplyDemandManager.h"

#include "SupplyDemand.h"
#include "SymbolicFunctions.h"
#include "MarketPlayer.h"
#include "MarketPlayerManager.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SDManagerTests : public ::testing::Test {
 protected:
  SupplyDemandManager manager;

  Commodity* commod;
  FunctionPtr demand;

  double capacity, cost;
  Producer* p1;
  Producer* p2;

  void initCommodity();

  
  MarketPlayerManager* mpmanager_;
  MarketPlayer* player1_;
  MarketPlayer* player2_;
  double cap1_, cap2_;
  void initMarketPlayers();

 public:

  virtual void SetUp();  
  virtual void TearDown();
};
