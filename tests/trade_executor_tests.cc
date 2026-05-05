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
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(TradeExecutorDatabaseTests, WrapperFunctionAndBasicRecording) {
  
  double orig_pref = 3.14;
  double test_trade_amt = 1.0;
  Request<Material>* req = Request<Material>::Create(fac_.mat, r1_);
  Bid<Material>* bid = Bid<Material>::Create(req, fac_.mat, s1_, false, orig_pref);
  
  Trade<Material> trade(req, bid, test_trade_amt);
  std::vector<Trade<Material>> trades;
  trades.push_back(trade);
  
  TradeExecutor<Material> exec(trades);
  
  // Test wrapper function - should not throw (this also records trades automatically)
  EXPECT_NO_THROW(exec.ExecuteTrades(ctx_));
  recorder_.Flush();
  
  // Verify bid object retains original preference
  EXPECT_DOUBLE_EQ(bid->preference(), orig_pref);
  
  // Query database and verify both preferences are the same (no ExchangeContext)
  cyclus::QueryResult qr = backend_->Query("Transactions", NULL);
  EXPECT_EQ(1, qr.rows.size()) << "Expected 1 transaction, got " << qr.rows.size();
  
  if (qr.rows.size() > 0) {
    double recorded_orig_cost = qr.GetVal<double>("BidCost", 0);
    double recorded_adj_cost = qr.GetVal<double>("AdjustedCost", 0);
    
    EXPECT_DOUBLE_EQ(recorded_orig_cost, 1.0 / orig_pref);
    EXPECT_DOUBLE_EQ(recorded_adj_cost, 1.0 / orig_pref); 
  }
  
  // Cleanup
  delete bid;
  delete req;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(TradeExecutorDatabaseTests, ExchangeContextWithAdjustedPreferences) {
  // Create one trade to test ExchangeContext functionality
  double orig_pref = 2.5;
  double trade_amt = 2.0;
  
  Request<Material>* req = Request<Material>::Create(fac_.mat, r1_);
  Bid<Material>* bid = Bid<Material>::Create(req, fac_.mat, s1_, false, orig_pref);
  
  std::vector<Trade<Material>> trades;
  trades.push_back(Trade<Material>(req, bid, trade_amt));
  
  // Create ExchangeContext with adjusted preferences
  ExchangeContext<Material> ex_ctx;
  ex_ctx.AddRequest(req);
  ex_ctx.AddBid(bid);
  
  // Set different adjusted preference
  double adj_pref = 4.2;
  
  // change the preference manually to new value
  ex_ctx.trader_prefs[r1_][req][bid] = adj_pref;
  
  TradeExecutor<Material> exec(trades);
  
  // should not throw (this also records trades automatically)
  EXPECT_NO_THROW(exec.ExecuteTrades(ctx_, &ex_ctx));
  recorder_.Flush();
  
  // Verify original bid preference is preserved
  EXPECT_DOUBLE_EQ(bid->preference(), orig_pref);
  
  // Verify adjusted preference in ExchangeContext
  EXPECT_DOUBLE_EQ(ex_ctx.trader_prefs[r1_][req][bid], adj_pref);
  
  // Query database and verify different original vs adjusted preferences
  cyclus::QueryResult qr = backend_->Query("Transactions", NULL);
  EXPECT_EQ(1, qr.rows.size()) << "Expected 1 transaction, got " << qr.rows.size();
  
  if (qr.rows.size() > 0) {
    double recorded_orig_cost = qr.GetVal<double>("BidCost", 0);
    double recorded_adj_cost = qr.GetVal<double>("AdjustedCost", 0);
    
    // Verify the costs match the expected preferences
    EXPECT_DOUBLE_EQ(recorded_orig_cost, 1.0 / orig_pref);
    EXPECT_DOUBLE_EQ(recorded_adj_cost, 1.0 / adj_pref);
    EXPECT_NE(recorded_orig_cost, recorded_adj_cost);  // Should be different
  }
  
  // Cleanup
  delete bid;
  delete req;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(TradeExecutorDatabaseTests, MixedPreferenceScenarios) {
  
  double explicit_pref = 2.8;
  double trade_amt = 1.0;
  Request<Material>* req = Request<Material>::Create(fac_.mat, r1_);
  
  // One bid with explicit preference, one with NaN (default)
  Bid<Material>* bid_explicit = Bid<Material>::Create(req, fac_.mat, s1_, false, explicit_pref);
  Bid<Material>* bid_nan = Bid<Material>::Create(req, fac_.mat, s2_);  // Default NaN preference
  
  std::vector<Trade<Material>> trades;
  trades.push_back(Trade<Material>(req, bid_explicit, trade_amt));
  trades.push_back(Trade<Material>(req, bid_nan, trade_amt));
  
  TradeExecutor<Material> exec(trades);
  
  // Test wrapper function with mixed preferences - should not throw (this also records trades automatically)
  EXPECT_NO_THROW(exec.ExecuteTrades(ctx_));
  recorder_.Flush();
  
  // Verify preference preservation
  EXPECT_DOUBLE_EQ(bid_explicit->preference(), explicit_pref);
  EXPECT_TRUE(std::isnan(bid_nan->preference()));
  
  // Query database
  cyclus::QueryResult qr = backend_->Query("Transactions", NULL);
  EXPECT_EQ(2, qr.rows.size()) << "Expected 2 transactions, got " 
            << qr.rows.size();
  
  // Cleanup
  delete bid_nan;
  delete bid_explicit;
  delete req;
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
