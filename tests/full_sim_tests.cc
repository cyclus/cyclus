#include <gtest/gtest.h>

#include "logger.h"
#include "test_context.h"

#include "full_sim_tests.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(FullSimTests, LoneTrader) {
  TestContext tc;

  TestObjFactory fac;
  
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
  
  TestTrader* base_supplier = new TestTrader(tc.get());
  TestTrader* supplier =
      dynamic_cast<TestTrader*>(base_supplier->Clone());
  supplier->Deploy();

  TestTrader* base_requester = new TestTrader(tc.get());
  TestTrader* requester =
      dynamic_cast<TestTrader*>(base_requester->Clone());
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(FullSimTests, Trade) {
  TestContext tc;
  TestObjFactory fac;
  bool is_requester = true;

  //  Logger::ReportLevel() = Logger::ToLogLevel("LEV_DEBUG2");
  
  TestTrader* base_supplier = new TestTrader(tc.get(), &fac, !is_requester);
  TestTrader* supplier =
      dynamic_cast<TestTrader*>(base_supplier->Clone());
  supplier->Deploy();

  TestTrader* base_requester = new TestTrader(tc.get(), &fac, is_requester);
  TestTrader* requester =
      dynamic_cast<TestTrader*>(base_requester->Clone());
  requester->Deploy();

  int nsteps = 3;
  
  tc.timer()->Initialize(tc.get(), nsteps);
  tc.timer()->RunSim(tc.get());

  EXPECT_EQ(nsteps, supplier->requests);
  EXPECT_EQ(nsteps, supplier->bids);
  EXPECT_EQ(0, supplier->accept);
  EXPECT_EQ(0, supplier->adjusts);
  EXPECT_EQ(nsteps, supplier->offer);
  
  EXPECT_EQ(nsteps, requester->requests);
  EXPECT_EQ(nsteps, requester->bids);
  EXPECT_EQ(nsteps, requester->accept);
  EXPECT_EQ(nsteps, requester->adjusts);
  EXPECT_EQ(0, requester->offer);

  // obs, exp
  Trade<Material> exp_trade(requester->req, supplier->bid,
                            supplier->bid->offer()->quantity());
  Material::Ptr exp_mat = fac.mat;
  EXPECT_EQ(supplier->req, requester->req);
  EXPECT_EQ(requester->bid, supplier->bid);
  EXPECT_EQ(requester->obs_trade, exp_trade);
  EXPECT_EQ(supplier->obs_trade, exp_trade);
  EXPECT_EQ(requester->mat, exp_mat);
  
  delete supplier;
  delete requester;
}

}// namespace cyclus
