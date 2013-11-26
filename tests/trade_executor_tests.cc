#include <gtest/gtest.h>

#include "bid.h"
#include "material.h"
#include "mock_facility.h"
#include "request.h"
#include "resource_helpers.h"
#include "test_context.h"
#include "trade.h"
#include "trader.h"
#include "trade_executor.h"

using cyclus::Bid;
using cyclus::Material;
using cyclus::Request;
using cyclus::TestContext;
using cyclus::Trade;
using cyclus::Trader;
using cyclus::TradeExecutor;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TradeExecutorTests : public ::testing::Test {
 public:
  TestContext tc;
  MockFacility* s1;
  MockFacility* s2;
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
    s1 = new MockFacility(tc.get());
    s2 = new MockFacility(tc.get());
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
TEST_F(TradeExecutorTests, SupplierMap) {
  TradeExecutor<Material> exec(trades);
  exec.__GroupTradesBySupplier();
  std::map<Trader*, std::vector< Trade<Material> > > obs =
      exec.trades_by_supplier_;
  std::map<Trader*, std::vector< Trade<Material> > > exp;
  exp[s1].push_back(t1);
  exp[s2].push_back(t2);
  exp[s2].push_back(t3);

  EXPECT_EQ(obs, exp);
}
