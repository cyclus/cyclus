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
#include "trade.h"
#include "trade_executor.h"
#include "trader.h"

using cyclus::Bid;
using cyclus::Context;
using cyclus::Material;
using cyclus::Model;
using cyclus::Request;
using cyclus::TestContext;
using cyclus::Trade;
using cyclus::TradeExecutor;
using cyclus::Trader;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TestSender : public MockFacility {
 public:
  TestSender(Context* ctx, Material::Ptr mat)
    : MockFacility(ctx),
      Model(ctx),
      mat(mat) { };
  
  virtual cyclus::Model* Clone() {
    TestSender* m = new TestSender(*this);
    m->InitFrom(this);
    m->mat = mat;
    return m;
  };

  virtual void PopulateMatlTradeResponses(
    const std::vector< Trade<Material> >& trades,
    std::vector<std::pair<Trade<Material>, Material::Ptr> >& responses) {
    std::vector< Trade<Material> >::const_iterator it;
    for (it = trades.begin(); it != trades.end(); ++it) {
      responses.push_back(std::make_pair(*it, mat));
    }
  }
  
  Material::Ptr mat;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TradeExecutorTests : public ::testing::Test {
 public:
  TestContext tc;
  TestSender* s1;
  TestSender* s2;
  MockFacility* r1;
  MockFacility* r2;
  Material::Ptr mat;
  double amt;
  Request<Material>::Ptr req1, req2;
  Bid<Material>::Ptr bid1, bid2, bid3;
  Trade<Material> t1, t2, t3;
  std::vector< Trade<Material> > trades;
  
  virtual void SetUp() {
    mat = test_helpers::get_mat();
    amt = 4.5; // some magic number..
    s1 = new TestSender(tc.get(), mat);
    s2 = new TestSender(tc.get(), mat);
    r1 = new MockFacility(tc.get());
    r2 = new MockFacility(tc.get());

    req1 = Request<Material>::Ptr(new Request<Material>(mat, r1));
    req2 = Request<Material>::Ptr(new Request<Material>(mat, r2));
    // supplier 1 makes a single bid for req1
    bid1 = Bid<Material>::Ptr(new Bid<Material>(req1, mat, s1));
    // supplier 2 makes a bid for req1 and req2
    bid2 = Bid<Material>::Ptr(new Bid<Material>(req1, mat, s2));
    bid3 = Bid<Material>::Ptr(new Bid<Material>(req2, mat, s2));

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
  exec.__GroupTradesBySupplier();
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
  exec.__GroupTradesBySupplier();  
  exec.__GetTradeResponses();

  std::map<Trader*,
           std::vector< std::pair<Trade<Material>, Material::Ptr> > >
      trades_by_requester;
  trades_by_requester[r1].push_back(std::make_pair(t1, mat));
  trades_by_requester[r1].push_back(std::make_pair(t2, mat));
  trades_by_requester[r2].push_back(std::make_pair(t3, mat));
  EXPECT_EQ(exec.trades_by_requester_, trades_by_requester);
  
  std::map<std::pair<Trader*, Trader*>,
           std::vector< std::pair<Trade<Material>, Material::Ptr> > > all_trades;
  all_trades[std::make_pair(s1, r1)].push_back(std::make_pair(t1, mat));
  all_trades[std::make_pair(s2, r1)].push_back(std::make_pair(t2, mat));
  all_trades[std::make_pair(s2, r2)].push_back(std::make_pair(t3, mat));
  EXPECT_EQ(exec.all_trades_, all_trades);
}
