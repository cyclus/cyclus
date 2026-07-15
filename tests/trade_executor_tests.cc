#include <map>
#include <set>
#include <utility>
#include <vector>
#include <sstream>

#include <gtest/gtest.h>

#include "agent.h"
#include "bid.h"
#include "context.h"
#include "exchange_context.h"
#include "material.h"
#include "request.h"
#include "resource_helpers.h"
#include "test_context.h"
#include "test_agents/test_facility.h"
#include "test_trader.h"
#include "trade.h"
#include "trade_executor.h"
#include "trader.h"

using namespace cyclus;
using cyclus::Bid;
using cyclus::Context;
using cyclus::ExchangeContext;
using cyclus::Material;
using cyclus::Agent;
using cyclus::Request;
using cyclus::TestContext;
using cyclus::TestObjFactory;
using cyclus::TestTrader;
using cyclus::Trade;
using cyclus::TradeExecutor;
using cyclus::Trader;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TradeExecutorTests : public ::testing::Test {
 public:
  TestContext tc;
  TestTrader* s1;
  TestTrader* s2;
  TestTrader* r1;
  TestTrader* r2;
  TestObjFactory fac;

  double amt;
  Request<Material>* req1;
  Request<Material>* req2;
  Bid<Material>* bid1;
  Bid<Material>* bid2;
  Bid<Material>* bid3;
  Trade<Material> t1, t2, t3;
  std::vector< Trade<Material> > trades;

  virtual void SetUp() {
    amt = 4.5;  // some magic number..
    s1 = new TestTrader(tc.get(), &fac);
    s2 = new TestTrader(tc.get(), &fac);
    r1 = new TestTrader(tc.get(), &fac);
    r2 = new TestTrader(tc.get(), &fac);

    req1 = Request<Material>::Create(fac.mat, r1);
    req2 = Request<Material>::Create(fac.mat, r2);
    // supplier 1 makes a single bid for req1
    bid1 = Bid<Material>::Create(req1, fac.mat, s1);
    // supplier 2 makes a bid for req1 and req2
    bid2 = Bid<Material>::Create(req1, fac.mat, s2);
    bid3 = Bid<Material>::Create(req2, fac.mat, s2);

    t1 = Trade<Material>(req1, bid1, amt);
    t2 = Trade<Material>(req1, bid2, amt);
    t3 = Trade<Material>(req2, bid3, amt);
    trades.push_back(t1);
    trades.push_back(t2);
    trades.push_back(t3);
  }

  virtual void TearDown() {
    delete bid3;
    delete bid2;
    delete bid1;
    delete req2;
    delete req1;
    delete r2;
    delete r1;
    delete s2;
    delete s1;
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TradeExecutorDatabaseTests : public ::testing::Test {
  protected:
   virtual void SetUp() {
     // Set up database backend for testing. NOTE, this approach gives us more
     // fine grained control over the database than mock_sim, which would not
     // allow the same kind of targeted function testing that we do here.
     path_ = ":memory:";  // In-memory database
     backend_ = new cyclus::SqliteBack(path_);
     recorder_.RegisterBackend(backend_);
     
     timer_ = new cyclus::Timer();
     ctx_ = new cyclus::Context(timer_, &recorder_);
     
     // Create test objects (s = supplier, r = requester)
     s1_ = new TestTrader(ctx_, &fac_);
     s2_ = new TestTrader(ctx_, &fac_);
     r1_ = new TestTrader(ctx_, &fac_);
     r2_ = new TestTrader(ctx_, &fac_);
   }
 
   virtual void TearDown() {
     delete r2_;
     delete r1_;
     delete s2_;
     delete s1_;
     delete ctx_;
     delete timer_;
     recorder_.Close();
     delete backend_;
   }
 
   // Database setup
   std::string path_;
   cyclus::SqliteBack* backend_;
   cyclus::Recorder recorder_;
   cyclus::Timer* timer_;
   cyclus::Context* ctx_;
   
   // Test objects
   TestObjFactory fac_;
   TestTrader* s1_;
   TestTrader* s2_;
   TestTrader* r1_;
   TestTrader* r2_;
 };

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(TradeExecutorTests, SupplierGrouping) {
  TradeExecutor<Material> exec(trades);
  GroupTradesBySupplier(exec.trade_ctx(), trades);
  std::map<Trader*, std::vector< Trade<Material> > > obs =
      exec.trade_ctx().trades_by_supplier;
  std::map<Trader*, std::vector< Trade<Material> > > exp;
  exp[s1].push_back(t1);
  exp[s2].push_back(t2);
  exp[s2].push_back(t3);

  EXPECT_EQ(obs, exp);

  std::set<Trader*> requesters;
  std::set<Trader*> suppliers;
  requesters.insert(r1);
  requesters.insert(r2);
  suppliers.insert(s1);
  suppliers.insert(s2);
  EXPECT_EQ(exec.trade_ctx().requesters, requesters);
  EXPECT_EQ(exec.trade_ctx().suppliers, suppliers);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(TradeExecutorTests, SupplierResponses) {
  TradeExecutor<Material> exec(trades);
  GroupTradesBySupplier(exec.trade_ctx(), trades);
  GetTradeResponses(exec.trade_ctx());

  std::map<Trader*,
           std::vector< std::pair<Trade<Material>, Material::Ptr> > >
      by_req_obs = exec.trade_ctx().trades_by_requester;
  EXPECT_NE(std::find(by_req_obs[r1].begin(),
                      by_req_obs[r1].end(),
                      std::make_pair(t1, fac.mat)),
            by_req_obs[r1].end());
  EXPECT_NE(std::find(by_req_obs[r1].begin(),
                      by_req_obs[r1].end(),
                      std::make_pair(t2, fac.mat)),
            by_req_obs[r1].end());
  EXPECT_NE(std::find(by_req_obs[r2].begin(),
                      by_req_obs[r2].end(),
                      std::make_pair(t3, fac.mat)),
            by_req_obs[r2].end());

  std::map<std::pair<Trader*, Trader*>,
           std::vector< std::pair<Trade<Material>, Material::Ptr> > >
      all_t_obs = exec.trade_ctx().all_trades;
  EXPECT_NE(std::find(all_t_obs[std::make_pair(s1, r1)].begin(),
                      all_t_obs[std::make_pair(s1, r1)].end(),
                      std::make_pair(t1, fac.mat)),
            all_t_obs[std::make_pair(s1, r1)].end());
  EXPECT_NE(std::find(all_t_obs[std::make_pair(s2, r1)].begin(),
                      all_t_obs[std::make_pair(s2, r1)].end(),
                      std::make_pair(t2, fac.mat)),
            all_t_obs[std::make_pair(s2, r1)].end());
  EXPECT_NE(std::find(all_t_obs[std::make_pair(s2, r2)].begin(),
                      all_t_obs[std::make_pair(s2, r2)].end(),
                      std::make_pair(t3, fac.mat)),
            all_t_obs[std::make_pair(s2, r2)].end());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(TradeExecutorTests, WholeShebang) {
  TradeExecutor<Material> exec(trades);
  exec.ExecuteTrades();
  EXPECT_EQ(s1->offer, 1);
  EXPECT_EQ(s1->accept, 0);
  EXPECT_EQ(s2->offer, 2);
  EXPECT_EQ(s2->accept, 0);
  EXPECT_EQ(r1->offer, 0);
  EXPECT_EQ(r1->accept, 2);
  EXPECT_EQ(r2->offer, 0);
  EXPECT_EQ(r2->accept, 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(TradeExecutorTests, NoThrowWriting) {
  TradeExecutor<Material> exec(trades);
  exec.ExecuteTrades();
  EXPECT_NO_THROW(exec.RecordTrades(tc.get()));
  
  // Test that trades between different agents don't trigger warnings
  warn_as_error = true;
  TradeExecutor<Material> warning_exec(trades);
  EXPECT_NO_THROW(warning_exec.ExecuteTrades(tc.get()));
  warn_as_error = false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(TradeExecutorTests, SelfTradingWarningIssued) {
  Request<Material>* req = Request<Material>::Create(fac.mat, s1, fac.commod);
  Bid<Material>* bid = Bid<Material>::Create(req, fac.mat, s1);

  std::vector< Trade<Material> > self_trades;
  self_trades.push_back(Trade<Material>(req, bid, amt));

  TradeExecutor<Material> executor(self_trades);

  warn_as_error = true;
  bool self_trade_warned = false;
  std::string warning_msg;
  try {
    executor.ExecuteTrades(tc.get());
  }
  catch (const cyclus::StateError& e) {
    self_trade_warned = true;
    warning_msg = e.what();
  }
  catch (...) {
    warn_as_error = false;
    throw;
  }
  warn_as_error = false;

  std::string expected_id = std::to_string(s1->id());
  EXPECT_TRUE(self_trade_warned);
  EXPECT_TRUE(warning_msg.find(expected_id) != std::string::npos);

  delete bid;
  delete req;
}

TEST_F(TradeExecutorDatabaseTests, WrapperFunctionAndBasicRecording) {
  
  double orig_unit_cost = 3.14;
  double test_trade_amt = 1.0;
  Request<Material>* req = Request<Material>::Create(fac_.mat, r1_);
  Bid<Material>* bid = Bid<Material>::Create(req, fac_.mat, s1_, false, orig_unit_cost);
  
  Trade<Material> trade(req, bid, test_trade_amt);
  std::vector<Trade<Material>> trades;
  trades.push_back(trade);
  
  TradeExecutor<Material> exec(trades);
  
  // Test wrapper function - should not throw (this also records trades automatically)
  EXPECT_NO_THROW(exec.ExecuteTrades(ctx_));
  recorder_.Flush();
  
  // Verify bid object retains original cost
  EXPECT_DOUBLE_EQ(bid->unit_cost(), orig_unit_cost);
  
  // Query database and verify UnitCost and UnitCostMod are recorded (no ExchangeContext means no adjustments)
  cyclus::QueryResult qr = backend_->Query("Transactions", NULL);
  EXPECT_EQ(1, qr.rows.size()) << "Expected 1 transaction, got " << qr.rows.size();
  
  if (qr.rows.size() > 0) {
    double recorded_cost = qr.GetVal<double>("UnitCost", 0);
    double recorded_mod = qr.GetVal<double>("UnitCostMod", 0);
    
    // UnitCost should equal val from bid, UnitCostMod should equal value from request (default = 0.0)
    EXPECT_DOUBLE_EQ(recorded_cost, orig_unit_cost);
    EXPECT_DOUBLE_EQ(recorded_mod, cyclus::kDefaultUnitCostMod);
  }
  
  // Cleanup
  delete bid;
  delete req;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(TradeExecutorDatabaseTests, ExchangeContextWithAdjustedArcCost) {
  // Create one trade to test ExchangeContext functionality
  double orig_unit_cost = 2.5;
  double trade_amt = 2.0;
  
  Request<Material>* req = Request<Material>::Create(fac_.mat, r1_);
  Bid<Material>* bid = Bid<Material>::Create(req, fac_.mat, s1_, false, orig_unit_cost);
  
  std::vector<Trade<Material>> trades;
  trades.push_back(Trade<Material>(req, bid, trade_amt));
  
  // Create ExchangeContext with adjusted arc cost
  ExchangeContext<Material> ex_ctx;
  ex_ctx.AddRequest(req);
  ex_ctx.AddBid(bid);
  
  // Set different adjusted arc cost
  double adj_arc_cost = 4.2;
  
  // Set adjusted arc cost in ExchangeContext
  ex_ctx.trader_arc_costs[r1_][req][bid] = adj_arc_cost;
  
  TradeExecutor<Material> exec(trades);
  
  // should not throw (this also records trades automatically)
  EXPECT_NO_THROW(exec.ExecuteTrades(ctx_, &ex_ctx));
  recorder_.Flush();
  
  // Verify original bid unit cost is preserved
  EXPECT_DOUBLE_EQ(bid->unit_cost(), orig_unit_cost);
  
  // Query database and verify adjusted unit cost and unit cost modifier are recorded
  cyclus::QueryResult qr = backend_->Query("Transactions", NULL);
  EXPECT_EQ(1, qr.rows.size()) << "Expected 1 transaction, got " << qr.rows.size();
  
  if (qr.rows.size() > 0) {
    double recorded_cost = qr.GetVal<double>("UnitCost", 0);
    double recorded_value = qr.GetVal<double>("UnitCostMod", 0);
    double recorded_arc_cost = qr.GetVal<double>("ArcCost", 0);

    // We changed the arc_cost directly here, so the original unit cost/mod
    // should persist, with a new adjusted_arc_cost
    EXPECT_DOUBLE_EQ(recorded_cost, orig_unit_cost);
    EXPECT_DOUBLE_EQ(recorded_value, cyclus::kDefaultUnitCostMod);
    EXPECT_DOUBLE_EQ(recorded_arc_cost, adj_arc_cost);
  }
  
  // Cleanup
  delete bid;
  delete req;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(TradeExecutorDatabaseTests, MixedCostScenarios) {
  
  double explicit_cost = 2.8;
  double trade_amt = 1.0;
  Request<Material>* req = Request<Material>::Create(fac_.mat, r1_);
  
  // One bid with explicit unit cost, one with default value
  Bid<Material>* bid_explicit = Bid<Material>::Create(req, fac_.mat, s1_, false, explicit_cost);
  Bid<Material>* bid_default = Bid<Material>::Create(req, fac_.mat, s2_);  // Default cost
  
  std::vector<Trade<Material>> trades;
  trades.push_back(Trade<Material>(req, bid_explicit, trade_amt));
  trades.push_back(Trade<Material>(req, bid_default, trade_amt));
  
  TradeExecutor<Material> exec(trades);
  
  // Test wrapper function with mixed unit costs - should not throw (this also records trades automatically)
  EXPECT_NO_THROW(exec.ExecuteTrades(ctx_));
  recorder_.Flush();
  
  // Verify unit cost preservation
  EXPECT_DOUBLE_EQ(bid_explicit->unit_cost(), explicit_cost);
  EXPECT_DOUBLE_EQ(bid_default->unit_cost(), cyclus::kDefaultUnitCost);
  
  // Query database
  cyclus::QueryResult qr = backend_->Query("Transactions", NULL);
  ASSERT_EQ(2, qr.rows.size()) << "Expected 2 transactions, got "
                                << qr.rows.size();

  std::map<double, int> seen_unit_costs;

  // Check to make sure that the costs we expect are actually recorded
  // using a for loop because we can't be certain which order they're
  // recorded in.
  for (int i = 0; i < qr.rows.size(); ++i) {
    double unit_cost = qr.GetVal<double>("UnitCost", i);
    double unit_cost_mod = qr.GetVal<double>("UnitCostMod", i);
    double arc_cost = qr.GetVal<double>("ArcCost", i);

    seen_unit_costs[unit_cost]++;

    EXPECT_DOUBLE_EQ(cyclus::kDefaultUnitCostMod, unit_cost_mod);
    EXPECT_DOUBLE_EQ(unit_cost + unit_cost_mod, arc_cost);
  }

  EXPECT_EQ(1, seen_unit_costs[explicit_cost]);
  EXPECT_EQ(1, seen_unit_costs[cyclus::kDefaultUnitCost]);
  
  // Cleanup
  delete bid_default;
  delete bid_explicit;
  delete req;
}
