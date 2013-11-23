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
    
  trader->Deploy(trader);

  int nsteps = 2;
  
  tc.timer()->Initialize(tc.get(), nsteps);
  tc.timer()->RunSim(tc.get());

  EXPECT_EQ(nsteps, trader->requests);
  EXPECT_EQ(nsteps, trader->bids);
  EXPECT_EQ(0, trader->adjusts);
  EXPECT_EQ(0, trader->accept);
  
  delete trader;
}

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TEST(FullSimTests, LoneSupplier) {
//   // TestContext tc;
  
//   // TestSupplier* base_supplier = new TestSupplier(tc.get());
//   // TestSupplier* supplier =
//   //     dynamic_cast<TestSupplier*>(base_supplier->Clone());
  
//   // supplier->Deploy(supplier);

//   // int nsteps = 2;
  
//   // tc.timer()->Initialize(tc.get(), nsteps);
//   // tc.timer()->RunSim(tc.get());

//   // EXPECT_EQ(nsteps, supplier->requests);
//   // EXPECT_EQ(nsteps, supplier->bids);
//   // EXPECT_EQ(0, supplier->accept);
  
//   // delete supplier;
// }

}// namespace cyclus
