#include <map>
#include <set>
#include <utility>
#include <vector>
#include <sstream>

#include <gtest/gtest.h>

#include "agent.h"
#include "bid.h"
#include "context.h"
#include "material.h"
#include "request.h"
#include "resource_helpers.h"
#include "test_context.h"
#include "test_agents/test_facility.h"
#include "test_trader.h"
#include "self_trading_test_facility.h"
#include "trade.h"
#include "trade_executor.h"
#include "trader.h"
#include "pyne.h"

using namespace cyclus;
using pyne::nucname::id;

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
class SelfTradingWarningTest : public ::testing::Test {
 public:
  virtual void SetUp() {

    tc_ = std::make_unique<TestContext>();
    
    facility_ = new SelfTradingTestFacility(tc_->get());
    
    facility_->Build(nullptr);
    facility_->EnterNotify();

    // Create a second test facility
    facility2_ = new SelfTradingTestFacility(tc_->get());
    facility2_->Build(nullptr);
    facility2_->EnterNotify();
    
    CompMap v;
    v[id("u235")] = 1;
    double trade_amt = 100;

    Composition::Ptr test_comp_ = Composition::CreateFromAtom(v);
    test_mat_ = Material::CreateUntracked(trade_amt, test_comp_);
  }

  virtual void TearDown() {
    delete facility_;
    delete facility2_;
  }

 protected:
  std::unique_ptr<TestContext> tc_;
  SelfTradingTestFacility* facility_;
  SelfTradingTestFacility* facility2_;
  Material::Ptr test_mat_;
  
  // Test constants
  static constexpr double kTestTradeAmount = 50.0;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SelfTradingWarningTest, SelfTradingWarningIssued) {
  warn_as_error = true;

  // Create a trade where the same facility is both supplier and requester
  Request<Material>* req = 
      Request<Material>::Create(test_mat_, facility_, "Uranium");
  Bid<Material>* bid = 
      Bid<Material>::Create(req, test_mat_, facility_);
  
  Trade<Material> trade(req, bid, kTestTradeAmount);
  std::vector<Trade<Material>> trades;
  trades.push_back(trade);
  
  // Execute the trade - this should trigger the warning
  TradeExecutor<Material> executor(trades);
  EXPECT_THROW(executor.ExecuteTrades(tc_->get()), cyclus::StateError);

  // Clean up
  warn_as_error = false;
  delete bid;
  delete req;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SelfTradingWarningTest, NoWarningForDifferentAgents) {
  
  // Create a trade between different facilities
  Request<Material>* req = 
      Request<Material>::Create(test_mat_, facility2_, "Uranium");
  Bid<Material>* bid = 
      Bid<Material>::Create(req, test_mat_, facility_);
  
  Trade<Material> trade(req, bid, kTestTradeAmount);
  std::vector<Trade<Material>> trades;
  trades.push_back(trade);
  
  
  // Execute the trade - this should NOT trigger the warning
  warn_as_error = true;
  TradeExecutor<Material> executor(trades);
  EXPECT_NO_THROW(executor.ExecuteTrades(tc_->get()));
  
  // Clean up
  warn_as_error = false;
  delete bid;
  delete req;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SelfTradingWarningTest, WarningIncludesCorrectAgentId) {
  
  
  
  // Create a trade where the same facility is both supplier and requester
  Request<Material>* req = 
      Request<Material>::Create(test_mat_, facility_, "Uranium");
  Bid<Material>* bid = 
      Bid<Material>::Create(req, test_mat_, facility_);
  
  Trade<Material> trade(req, bid, kTestTradeAmount);
  std::vector<Trade<Material>> trades;
  trades.push_back(trade);
  
  warn_as_error = true;
  TradeExecutor<Material> executor(trades);

  // Use a try/catch to check for the error message
  try {
  EXPECT_THROW(executor.ExecuteTrades(tc_->get()), cyclus::StateError);
  }
  catch (const cyclus::StateError& e) {
    std::string expected_id = std::to_string(facility_->id());
    EXPECT_TRUE(std::string(e.what()).find(expected_id) != std::string::npos);
  }
  
  // Clean up
  warn_as_error = false;
  delete bid;
  delete req;
}
