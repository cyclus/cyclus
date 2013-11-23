#include <gtest/gtest.h>

#include "test_context.h"

#include "full_sim_tests.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(FullSimTests, LoneTrader) {
  TestContext tc;
  
  TestTrader* base_trader = new TestTrader(tc.get());
  TestTrader* trader =
      dynamic_cast<TestTrader*>(base_trader->Clone());
    
  trader->Deploy();

  int nsteps = 5;
  
  tc.timer()->Initialize(tc.get(), nsteps);
  tc.timer()->RunSim(tc.get());

  EXPECT_EQ(nsteps, trader->requests);
  EXPECT_EQ(nsteps, trader->bids);
  EXPECT_EQ(0, trader->adjusts);
  EXPECT_EQ(0, trader->accept);
  
  delete trader;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(FullSimTests, NullTrade) {
  TestContext tc;
  
  TestSupplier* base_supplier = new TestSupplier(tc.get());
  TestSupplier* supplier =
      dynamic_cast<TestSupplier*>(base_supplier->Clone());
  supplier->Deploy();

  TestRequester* base_requester = new TestRequester(tc.get());
  TestRequester* requester =
      dynamic_cast<TestRequester*>(base_requester->Clone());
  requester->Deploy();

  int nsteps = 2;
  
  tc.timer()->Initialize(tc.get(), nsteps);
  tc.timer()->RunSim(tc.get());

  EXPECT_EQ(nsteps, supplier->requests);
  EXPECT_EQ(nsteps, supplier->bids);
  EXPECT_EQ(0, supplier->accept);
  EXPECT_EQ(0, supplier->adjusts);
  
  EXPECT_EQ(nsteps, requester->requests);
  EXPECT_EQ(nsteps, requester->bids);
  EXPECT_EQ(0, requester->accept);
  EXPECT_EQ(0, requester->adjusts);
  
  delete supplier;
  delete requester;
}

}// namespace cyclus
