#include <map>
#include <set>
#include <utility>
#include <vector>

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
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(TradeExecutorDatabaseTests, WrapperFunctionAndBasicRecording) {
  // Set up database backend for testing. NOTE, this approach gives us more
  // fine grained control over the database than mock_sim, which would not
  // allow the same kind of targeted function testing that we do here.
  std::string path = ":memory:";  // In-memory database
  cyclus::SqliteBack* b = new cyclus::SqliteBack(path);
  cyclus::Recorder rec;
  rec.RegisterBackend(b);
  
  cyclus::Timer ti;
  cyclus::Context* ctx = new cyclus::Context(&ti, &rec);
  
  // Create test objects
  TestObjFactory fac;
  TestTrader* s1 = new TestTrader(ctx, &fac);
  TestTrader* r1 = new TestTrader(ctx, &fac);
  
  double orig_pref = 3.14;
  double test_trade_amt = 1.0;
  Request<Material>* req = Request<Material>::Create(fac.mat, r1);
  Bid<Material>* bid = Bid<Material>::Create(req, fac.mat, s1, false, orig_pref);
  
  Trade<Material> trade(req, bid, test_trade_amt);
  std::vector<Trade<Material>> trades;
  trades.push_back(trade);
  
  TradeExecutor<Material> exec(trades);
  
  // Test wrapper function - should not throw (this also records trades automatically)
  EXPECT_NO_THROW(exec.ExecuteTrades(ctx));  // Single parameter version
  rec.Flush();
  
  // Verify bid object retains original preference
  EXPECT_DOUBLE_EQ(bid->preference(), orig_pref);
  
  // Query database and verify both preferences are the same (no ExchangeContext)
  cyclus::QueryResult qr = b->Query("Transactions", NULL);
  EXPECT_EQ(1, qr.rows.size()) << "Expected 1 transaction, got " << qr.rows.size();
  
  if (qr.rows.size() > 0) {
    double recorded_orig_cost = qr.GetVal<double>("BidCost", 0);
    double recorded_adj_cost = qr.GetVal<double>("AdjustedCost", 0);
    
    EXPECT_DOUBLE_EQ(recorded_orig_cost, 1.0 / orig_pref);
    EXPECT_DOUBLE_EQ(recorded_adj_cost, 1.0 / orig_pref);
    EXPECT_DOUBLE_EQ(recorded_orig_cost, recorded_adj_cost);
  }
  
  // Cleanup
  delete bid;
  delete req;
  delete r1;
  delete s1;
  rec.Close();
  delete ctx;
  delete b;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(TradeExecutorDatabaseTests, ExchangeContextWithAdjustedPreferences) {
  // Set up database backend for testing, again, this appproach is used to
  // give us more fine grained control over the database than mock_sim.
  std::string path = ":memory:";  // In-memory database
  cyclus::SqliteBack* b = new cyclus::SqliteBack(path);
  cyclus::Recorder rec;
  rec.RegisterBackend(b);
  
  cyclus::Timer ti;
  cyclus::Context* ctx = new cyclus::Context(&ti, &rec);
  
  // Create test objects
  TestObjFactory fac;
  TestTrader* s1 = new TestTrader(ctx, &fac);
  TestTrader* s2 = new TestTrader(ctx, &fac);
  TestTrader* r1 = new TestTrader(ctx, &fac);
  TestTrader* r2 = new TestTrader(ctx, &fac);
  
  // Create multiple trades with different preferences
  double orig_pref1 = 2.5;
  double orig_pref2 = 3.7;
  double orig_pref3 = 1.2;

  double trade_amt1 = 2.0;
  double trade_amt2 = 1.5;
  double trade_amt3 = 3.0;
  
  Request<Material>* req1 = Request<Material>::Create(fac.mat, r1);
  Request<Material>* req2 = Request<Material>::Create(fac.mat, r2);
  
  Bid<Material>* bid1 = Bid<Material>::Create(req1, fac.mat, s1, false, orig_pref1);
  Bid<Material>* bid2 = Bid<Material>::Create(req1, fac.mat, s2, false, orig_pref2);
  Bid<Material>* bid3 = Bid<Material>::Create(req2, fac.mat, s2, false, orig_pref3);
  
  std::vector<Trade<Material>> trades;
  trades.push_back(Trade<Material>(req1, bid1, trade_amt1));
  trades.push_back(Trade<Material>(req1, bid2, trade_amt2));
  trades.push_back(Trade<Material>(req2, bid3, trade_amt3));
  
  // Create ExchangeContext with adjusted preferences
  ExchangeContext<Material> ex_ctx;
  ex_ctx.AddRequest(req1);
  ex_ctx.AddRequest(req2);
  ex_ctx.AddBid(bid1);
  ex_ctx.AddBid(bid2);
  ex_ctx.AddBid(bid3);
  
  // Set different adjusted preferences
  double adj_pref1 = 4.2;
  double adj_pref2 = 1.8;
  double adj_pref3 = 5.1;
  
  ex_ctx.trader_prefs[r1][req1][bid1] = adj_pref1;
  ex_ctx.trader_prefs[r1][req1][bid2] = adj_pref2;
  ex_ctx.trader_prefs[r2][req2][bid3] = adj_pref3;
  
  TradeExecutor<Material> exec(trades);
  
  // Test two-parameter version - should not throw (this also records trades automatically)
  EXPECT_NO_THROW(exec.ExecuteTrades(ctx, &ex_ctx));
  rec.Flush();
  
  // Verify original bid preferences are preserved
  EXPECT_DOUBLE_EQ(bid1->preference(), orig_pref1);
  EXPECT_DOUBLE_EQ(bid2->preference(), orig_pref2);
  EXPECT_DOUBLE_EQ(bid3->preference(), orig_pref3);
  
  // Verify adjusted preferences in ExchangeContext
  EXPECT_DOUBLE_EQ(ex_ctx.trader_prefs[r1][req1][bid1], adj_pref1);
  EXPECT_DOUBLE_EQ(ex_ctx.trader_prefs[r1][req1][bid2], adj_pref2);
  EXPECT_DOUBLE_EQ(ex_ctx.trader_prefs[r2][req2][bid3], adj_pref3);
  
  // Query database and verify different original vs adjusted preferences
  cyclus::QueryResult qr = b->Query("Transactions", NULL);
  EXPECT_EQ(3, qr.rows.size()) << "Expected 3 transactions, got " << qr.rows.size();
  
  // Check each transaction's preferences
  std::vector<double> expected_orig = {orig_pref1, orig_pref2, orig_pref3};
  std::vector<double> expected_adj = {adj_pref1, adj_pref2, adj_pref3};
  
  for (int i = 0; i < qr.rows.size(); i++) {
    double recorded_orig_cost = qr.GetVal<double>("BidCost", i);
    double recorded_adj_cost = qr.GetVal<double>("AdjustedCost", i);
    
    // Find which expected preference this matches
    bool found_match = false;
    for (size_t j = 0; j < expected_orig.size(); j++) {
      if (std::abs(recorded_orig_cost - 1.0 / expected_orig[j]) < 1e-10) {
        EXPECT_DOUBLE_EQ(recorded_adj_cost, 1.0 / expected_adj[j]);
        EXPECT_NE(recorded_orig_cost, recorded_adj_cost);  // Should be different
        found_match = true;
        break;
      }
    }
    EXPECT_TRUE(found_match) << "Recorded bid cost " << recorded_orig_cost 
                             << " doesn't match any expected value";
  }
  
  // Cleanup
  delete bid3;
  delete bid2;
  delete bid1;
  delete req2;
  delete req1;
  delete r2;
  delete r1;
  delete s2;
  delete s1;
  rec.Close();
  delete ctx;
  delete b;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(TradeExecutorDatabaseTests, MixedPreferenceScenarios) {
  // Set up database backend for testing
  std::string path = ":memory:";  // In-memory database
  cyclus::SqliteBack* b = new cyclus::SqliteBack(path);
  cyclus::Recorder rec;
  rec.RegisterBackend(b);
  
  cyclus::Timer ti;
  cyclus::Context* ctx = new cyclus::Context(&ti, &rec);
  
  // Create test objects
  TestObjFactory fac;
  TestTrader* s1 = new TestTrader(ctx, &fac);
  TestTrader* s2 = new TestTrader(ctx, &fac);
  TestTrader* r1 = new TestTrader(ctx, &fac);
  
  double explicit_pref = 2.8;
  double trade_amt = 1.0;
  Request<Material>* req = Request<Material>::Create(fac.mat, r1);
  
  // One bid with explicit preference, one with NaN (default)
  Bid<Material>* bid_explicit = Bid<Material>::Create(req, fac.mat, s1, false, explicit_pref);
  Bid<Material>* bid_nan = Bid<Material>::Create(req, fac.mat, s2);  // Default NaN preference
  
  std::vector<Trade<Material>> trades;
  trades.push_back(Trade<Material>(req, bid_explicit, trade_amt));
  trades.push_back(Trade<Material>(req, bid_nan, trade_amt));
  
  TradeExecutor<Material> exec(trades);
  
  // Test wrapper function with mixed preferences - should not throw (this also records trades automatically)
  EXPECT_NO_THROW(exec.ExecuteTrades(ctx));
  rec.Flush();
  
  // Verify preference preservation
  EXPECT_DOUBLE_EQ(bid_explicit->preference(), explicit_pref);
  EXPECT_TRUE(std::isnan(bid_nan->preference()));
  
  // Query database
  cyclus::QueryResult qr = b->Query("Transactions", NULL);
  EXPECT_EQ(2, qr.rows.size()) << "Expected 2 transactions, got " 
            << qr.rows.size();
  
  // Cleanup
  delete bid_nan;
  delete bid_explicit;
  delete req;
  delete r1;
  delete s2;
  delete s1;
  rec.Close();
  delete ctx;
  delete b;
}

// This test was a part of a previous iteration of Trade testing, but its not
// clear if this throwing behavior is what we want. I'm leaving it here for now
// in case it needs to be picked up again. MJG - 11/26/13
// // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TEST(TradeTests, OfferThrow) {
//   TestContext tc;

//   Material::Ptr mat = get_mat();
//   Receiver* r = new Receiver(tc.get(), mat);
//   Request<Material>* req = Request<Material>::Create(mat, r);

//   Sender* s = new Sender(tc.get(), true);
//   Bid<Material>* bid = Bid<Material>::Create(req, mat, s);

//   Trade<Material> trade(req, bid, mat->quantity());
//   EXPECT_THROW(cyclus::ExecuteTrade(trade), cyclus::ValueError);
//   delete s;
//   delete r;
// }
