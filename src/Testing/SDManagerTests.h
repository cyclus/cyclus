// SDManagerTests.h
#include <gtest/gtest.h>
#include "CommodityTestHelper.h"

#include "SupplyDemandManager.h"
#include "SymbolicFunctions.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SDManagerTests : public ::testing::Test {
 public:
  SupplyDemandManager manager;
  FunctionPtr demand;
  CommodityTestHelper* helper;

  virtual void SetUp();  
  virtual void TearDown();
};
