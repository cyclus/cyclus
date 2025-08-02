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
#include "trade.h"
#include "trade_executor.h"
#include "trader.h"

using cyclus::Bid;
using cyclus::Context;
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Simple test facility for self-trading tests
class SelfTradingTestFacility : public cyclus::Facility {
 public:
  SelfTradingTestFacility(cyclus::Context* ctx) : cyclus::Facility(ctx) {}
  virtual ~SelfTradingTestFacility() {}

  virtual cyclus::Agent* Clone() { 
    return new SelfTradingTestFacility(context()); 
  }
  
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() { 
    return cyclus::Inventories(); 
  }

  void Tick() {}
  void Tock() {}

  // Simple implementation to support trading
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
      GetMatlRequests() {
    std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr> ports;
    cyclus::RequestPortfolio<cyclus::Material>::Ptr port(
        new cyclus::RequestPortfolio<cyclus::Material>());
    
    cyclus::Material::Ptr mat = cyclus::NewBlankMaterial(100);
    port->AddRequest(mat, this, "NaturalUranium");
    ports.insert(port);
    return ports;
  }

  virtual std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr>
      GetMatlBids(cyclus::CommodMap<cyclus::Material>::type& commod_requests) {
    std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr> ports;
    
    if (commod_requests.count("NaturalUranium") > 0) {
      cyclus::BidPortfolio<cyclus::Material>::Ptr port(
          new cyclus::BidPortfolio<cyclus::Material>());
      
      std::vector<cyclus::Request<cyclus::Material>*>& requests = 
          commod_requests.at("NaturalUranium");
      
      for (std::vector<cyclus::Request<cyclus::Material>*>::iterator it = 
               requests.begin(); it != requests.end(); ++it) {
        cyclus::Request<cyclus::Material>* req = *it;
        cyclus::Material::Ptr offer = cyclus::NewBlankMaterial(100);
        port->AddBid(req, offer, this);
      }
      ports.insert(port);
    }
    return ports;
  }

  virtual void GetMatlTrades(
      const std::vector<cyclus::Trade<cyclus::Material>>& trades,
      std::vector<std::pair<cyclus::Trade<cyclus::Material>, 
                           cyclus::Material::Ptr>>& responses) {
    for (std::vector<cyclus::Trade<cyclus::Material>>::const_iterator it = 
             trades.begin(); it != trades.end(); ++it) {
      cyclus::Material::Ptr response = cyclus::NewBlankMaterial(it->amt);
      responses.push_back(std::make_pair(*it, response));
    }
  }

  virtual void AcceptMatlTrades(
      const std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                                 cyclus::Material::Ptr>>& responses) {
    // Simple acceptance - just ignore the materials
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Test for self-trading warning functionality
class SelfTradingWarningTest : public ::testing::Test {
 public:
  virtual void SetUp() {
    // Create a test context
    tc_ = std::make_unique<cyclus::TestContext>();
    
    // Create a test facility that can trade with itself
    facility_ = new SelfTradingTestFacility(tc_->get());
    
    // Set up the facility
    facility_->Build(nullptr);
    facility_->EnterNotify();
  }

  virtual void TearDown() {
    delete facility_;
  }

 protected:
  std::unique_ptr<cyclus::TestContext> tc_;
  SelfTradingTestFacility* facility_;
};

TEST_F(SelfTradingWarningTest, SelfTradingWarningIssued) {
  // Capture stderr to check for warning messages
  std::stringstream captured_stderr;
  std::streambuf* original_stderr = std::cerr.rdbuf();
  std::cerr.rdbuf(captured_stderr.rdbuf());
  
  // Create a material for testing
  cyclus::CompMap v;
  v[922350000] = 1;  // U-235
  v[922380000] = 2;  // U-238
  cyclus::Composition::Ptr comp = cyclus::Composition::CreateFromAtom(v);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(100, comp);
  
  // Create a trade where the same facility is both supplier and requester
  cyclus::Request<cyclus::Material>* req = 
      cyclus::Request<cyclus::Material>::Create(mat, facility_, "NaturalUranium");
  cyclus::Bid<cyclus::Material>* bid = 
      cyclus::Bid<cyclus::Material>::Create(req, mat, facility_);
  
  cyclus::Trade<cyclus::Material> trade(req, bid, 50.0);
  std::vector<cyclus::Trade<cyclus::Material>> trades;
  trades.push_back(trade);
  
  // Execute the trade - this should trigger the warning
  cyclus::TradeExecutor<cyclus::Material> executor(trades);
  executor.ExecuteTrades(tc_->get());
  
  // Restore stderr
  std::cerr.rdbuf(original_stderr);
  
  // Check that the warning was issued
  std::string stderr_output = captured_stderr.str();
  EXPECT_TRUE(stderr_output.find("State Warning") != std::string::npos);
  EXPECT_TRUE(stderr_output.find("is trading with itself") != std::string::npos);
  EXPECT_TRUE(stderr_output.find("NaturalUranium") != std::string::npos);
  
  // Clean up
  delete bid;
  delete req;
}

TEST_F(SelfTradingWarningTest, NoWarningForDifferentAgents) {
  // Create a second test facility
  SelfTradingTestFacility* facility2 = new SelfTradingTestFacility(tc_->get());
  facility2->Build(nullptr);
  facility2->EnterNotify();
  
  // Capture stderr
  std::stringstream captured_stderr;
  std::streambuf* original_stderr = std::cerr.rdbuf();
  std::cerr.rdbuf(captured_stderr.rdbuf());
  
  // Create a trade between different facilities
  cyclus::CompMap v;
  v[922350000] = 1;
  v[922380000] = 2;
  cyclus::Composition::Ptr comp = cyclus::Composition::CreateFromAtom(v);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(100, comp);
  
  cyclus::Request<cyclus::Material>* req = 
      cyclus::Request<cyclus::Material>::Create(mat, facility2, "NaturalUranium");
  cyclus::Bid<cyclus::Material>* bid = 
      cyclus::Bid<cyclus::Material>::Create(req, mat, facility_);
  
  cyclus::Trade<cyclus::Material> trade(req, bid, 50.0);
  std::vector<cyclus::Trade<cyclus::Material>> trades;
  trades.push_back(trade);
  
  // Execute the trade - this should NOT trigger the warning
  cyclus::TradeExecutor<cyclus::Material> executor(trades);
  executor.ExecuteTrades(tc_->get());
  
  // Restore stderr
  std::cerr.rdbuf(original_stderr);
  
  // Check that no self-trading warning was issued
  std::string stderr_output = captured_stderr.str();
  EXPECT_TRUE(stderr_output.find("is trading with itself") == std::string::npos);
  
  // Clean up
  delete bid;
  delete req;
  delete facility2;
}

TEST_F(SelfTradingWarningTest, WarningIncludesCorrectAgentId) {
  // Capture stderr
  std::stringstream captured_stderr;
  std::streambuf* original_stderr = std::cerr.rdbuf();
  std::cerr.rdbuf(captured_stderr.rdbuf());
  
  // Create a material for testing
  cyclus::CompMap v;
  v[922350000] = 1;
  v[922380000] = 2;
  cyclus::Composition::Ptr comp = cyclus::Composition::CreateFromAtom(v);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(100, comp);
  
  // Create a trade where the same facility is both supplier and requester
  cyclus::Request<cyclus::Material>* req = 
      cyclus::Request<cyclus::Material>::Create(mat, facility_, "NaturalUranium");
  cyclus::Bid<cyclus::Material>* bid = 
      cyclus::Bid<cyclus::Material>::Create(req, mat, facility_);
  
  cyclus::Trade<cyclus::Material> trade(req, bid, 50.0);
  std::vector<cyclus::Trade<cyclus::Material>> trades;
  trades.push_back(trade);
  
  // Execute the trade
  cyclus::TradeExecutor<cyclus::Material> executor(trades);
  executor.ExecuteTrades(tc_->get());
  
  // Restore stderr
  std::cerr.rdbuf(original_stderr);
  
  // Check that the warning includes the correct agent ID
  std::string stderr_output = captured_stderr.str();
  std::string expected_id = std::to_string(facility_->id());
  EXPECT_TRUE(stderr_output.find(expected_id) != std::string::npos);
  
  // Clean up
  delete bid;
  delete req;
}
