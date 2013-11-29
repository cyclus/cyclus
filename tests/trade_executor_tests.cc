#include <gtest/gtest.h>

#include <map>
#include <set>
#include <utility>
#include <vector>

#include "bid.h"
#include "context.h"
#include "material.h"
#include "mock_facility.h"
#include "model.h"
#include "request.h"
#include "resource_helpers.h"
#include "test_context.h"
#include "test_trader.h"
#include "trade.h"
#include "trade_executor.h"
#include "trader.h"

using cyclus::Bid;
using cyclus::Context;
using cyclus::Material;
using cyclus::Model;
using cyclus::Request;
using cyclus::TestContext;
using cyclus::TestObjFactory;
using cyclus::TestTrader;
using cyclus::Trade;
using cyclus::TradeExecutor;
using cyclus::Trader;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TradeExecutorTests : public ::testing::Test {
 public:
  TestContext tc;
  TestTrader* s1;
  TestTrader* s2;
  TestTrader* r1;
  TestTrader* r2;
  TestObjFactory fac;
  
  double amt;
  Request<Material>::Ptr req1, req2;
  Bid<Material>::Ptr bid1, bid2, bid3;
  Trade<Material> t1, t2, t3;
  std::vector< Trade<Material> > trades;
  
  virtual void SetUp() {
    amt = 4.5; // some magic number..
    s1 = new TestTrader(tc.get(), &fac);
    s2 = new TestTrader(tc.get(), &fac);
    r1 = new TestTrader(tc.get(), &fac);
    r2 = new TestTrader(tc.get(), &fac);

    req1 = Request<Material>::Ptr(new Request<Material>(fac.mat, r1));
    req2 = Request<Material>::Ptr(new Request<Material>(fac.mat, r2));
    // supplier 1 makes a single bid for req1
    bid1 = Bid<Material>::Ptr(new Bid<Material>(req1, fac.mat, s1));
    // supplier 2 makes a bid for req1 and req2
    bid2 = Bid<Material>::Ptr(new Bid<Material>(req1, fac.mat, s2));
    bid3 = Bid<Material>::Ptr(new Bid<Material>(req2, fac.mat, s2));

    t1 = Trade<Material>(req1, bid1, amt);
    t2 = Trade<Material>(req1, bid2, amt);
    t3 = Trade<Material>(req2, bid3, amt);
    trades.push_back(t1);
    trades.push_back(t2);
    trades.push_back(t3);
  }
  
  virtual void TearDown() {
    delete s1;
    delete s2;
    delete r1;
    delete r2;
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(TradeExecutorTests, SupplierGrouping) {
  TradeExecutor<Material> exec(trades);
  exec.GroupTradesBySupplier_();
  std::map<Trader*, std::vector< Trade<Material> > > obs =
      exec.trades_by_supplier_;
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
  EXPECT_EQ(exec.requesters_, requesters);
  EXPECT_EQ(exec.suppliers_, suppliers);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(TradeExecutorTests, SupplierResponses) {
  TradeExecutor<Material> exec(trades);
  exec.GroupTradesBySupplier_();  
  exec.GetTradeResponses_();

  std::map<Trader*,
           std::vector< std::pair<Trade<Material>, Material::Ptr> > >
      trades_by_requester;
  trades_by_requester[r1].push_back(std::make_pair(t1, fac.mat));
  trades_by_requester[r1].push_back(std::make_pair(t2, fac.mat));
  trades_by_requester[r2].push_back(std::make_pair(t3, fac.mat));
  EXPECT_EQ(exec.trades_by_requester_, trades_by_requester);
  
  std::map<std::pair<Trader*, Trader*>,
           std::vector< std::pair<Trade<Material>, Material::Ptr> > > all_trades;
  all_trades[std::make_pair(s1, r1)].push_back(std::make_pair(t1, fac.mat));
  all_trades[std::make_pair(s2, r1)].push_back(std::make_pair(t2, fac.mat));
  all_trades[std::make_pair(s2, r2)].push_back(std::make_pair(t3, fac.mat));
  EXPECT_EQ(exec.all_trades_, all_trades);
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
//   Request<Material>::Ptr req(new Request<Material>(mat, r));
  
//   Sender* s = new Sender(tc.get(), true);
//   Bid<Material>::Ptr bid(new Bid<Material>(req, mat, s));
  
//   Trade<Material> trade(req, bid, mat->quantity());
//   EXPECT_THROW(cyclus::ExecuteTrade(trade), cyclus::ValueError);
//   delete s;
//   delete r;
// }
