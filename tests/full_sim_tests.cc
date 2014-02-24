#include <gtest/gtest.h>

#include "greedy_solver.h"
#include "logger.h"
#include "test_context.h"
#include "test_trader.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(FullSimTests, LoneTrader) {
  TestContext tc;
  GreedySolver* solver = new GreedySolver(); // context deletes
  tc.get()->solver(solver);
  TestObjFactory fac;
  
  TestTrader* base_trader = new TestTrader(tc.get());
  TestTrader* trader =
      dynamic_cast<TestTrader*>(base_trader->Clone());
    
  trader->Build();

  int nsteps = 5;
  
  tc.timer()->Initialize(tc.get(), nsteps);
  tc.timer()->RunSim(tc.get());

  EXPECT_EQ(nsteps, trader->requests);
  EXPECT_EQ(nsteps, trader->bids);
  EXPECT_EQ(0, trader->adjusts);
  EXPECT_EQ(0, trader->accept);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(FullSimTests, NullTrade) {
  TestContext tc;
  GreedySolver* solver = new GreedySolver(); // context deletes
  tc.get()->solver(solver);
  
  TestTrader* base_supplier = new TestTrader(tc.get());
  TestTrader* supplier =
      dynamic_cast<TestTrader*>(base_supplier->Clone());
  supplier->Build();

  TestTrader* base_requester = new TestTrader(tc.get());
  TestTrader* requester =
      dynamic_cast<TestTrader*>(base_requester->Clone());
  requester->Build();

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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(FullSimTests, Trade) {
  TestContext tc;
  GreedySolver* solver = new GreedySolver(); // context deletes
  tc.get()->solver(solver);
  TestObjFactory fac;
  bool is_requester = true;

  //  Logger::ReportLevel() = Logger::ToLogLevel("LEV_DEBUG2");
  
  TestTrader* base_supplier = new TestTrader(tc.get(), &fac, !is_requester);
  TestTrader* supplier =
      dynamic_cast<TestTrader*>(base_supplier->Clone());
  supplier->Build();

  TestTrader* base_requester = new TestTrader(tc.get(), &fac, is_requester);
  TestTrader* requester =
      dynamic_cast<TestTrader*>(base_requester->Clone());
  requester->Build();

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
}

}// namespace cyclus
