// SDManagerTests.h
#include <gtest/gtest.h>

#include "SupplyDemandManager.h"

#include "SupplyDemand.h"
#include "SymbolicFunctions.h"

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

 public:

  virtual void SetUp();  
  virtual void TearDown();
  
};
