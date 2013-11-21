#include <gtest/gtest.h>

#include "bid.h"
#include "context.h"
#include "error.h"
#include "material.h"
#include "mock_facility.h"
#include "model.h"
#include "request.h"
#include "resource_helpers.h"
#include "test_context.h"
#include "trade.h"
#include "trader_management.h"

using cyclus::Bid;
using cyclus::Context;
using cyclus::Material;
using cyclus::Request;
using cyclus::TestContext;
using cyclus::Trade;
using test_helpers::get_mat;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class Sender : public MockFacility {
 public:
  Sender(Context* ctx, bool adjust = false)
    : MockFacility(ctx),
      Model(ctx),
      i(0),
      adjust(adjust) { };
  
  virtual cyclus::Model* Clone() {
    Sender* m = new Sender(*this);
    m->InitFrom(this);
    m->i = i;
    m->adjust = adjust;
    return m;
  };

  Material::Ptr OfferMatlTrade(const Trade<Material>& trade) {
    ++i;
    Material::Ptr mat = trade.bid->offer();
    if (adjust) {
      mat = mat->ExtractQty(mat->quantity() - 0.1);
    }
    return mat;
  }

  bool adjust;
  int i;
};
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class Receiver : public MockFacility {
 public:
  Receiver(Context* ctx, Material::Ptr known)
    : MockFacility(ctx),
      Model(ctx),
      known(known) { };

  virtual cyclus::Model* Clone() {
    Receiver* m = new Receiver(*this);
    m->InitFrom(this);
    m->i = i;
    m->known = known;
    return m;
  };

  void AcceptMatlTrade(const Trade<Material>& trade, Material::Ptr rsrc) {
    if (rsrc == known) ++i;
  };

  int i;
  Material::Ptr known;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(TradeTests, Offer) {
  TestContext tc;
  
  Material::Ptr mat = get_mat();
  Receiver* r = new Receiver(tc.get(), mat);
  Request<Material>::Ptr req(new Request<Material>(mat, r));
  
  Sender* s = new Sender(tc.get());
  Bid<Material>::Ptr bid(new Bid<Material>(req, mat, s));
  
  Trade<Material> trade(req, bid, mat->quantity());
  EXPECT_EQ(0, s->i);
  Material::Ptr rsrc = cyclus::ExecTradeOffer(trade);
  EXPECT_EQ(bid->offer(), rsrc);
  EXPECT_EQ(1, s->i);
  delete s;
  delete r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(TradeTests, Accept) {
  TestContext tc;
  
  Material::Ptr mat = get_mat();
  Receiver* r = new Receiver(tc.get(), mat);
  Request<Material>::Ptr req(new Request<Material>(mat, r));
  
  Sender* s = new Sender(tc.get());
  Bid<Material>::Ptr bid(new Bid<Material>(req, mat, s));
  
  Trade<Material> trade(req, bid, mat->quantity());
  EXPECT_EQ(0, r->i);
  cyclus::ExecTradeAccept(trade, mat);
  EXPECT_EQ(1, r->i); // known == mat
  
  cyclus::ExecTradeAccept(trade, get_mat()); 
  EXPECT_EQ(1, r->i); // known != new get_mat()
  
  delete s;
  delete r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(TradeTests, OfferThrow) {
  TestContext tc;
  
  Material::Ptr mat = get_mat();
  Receiver* r = new Receiver(tc.get(), mat);
  Request<Material>::Ptr req(new Request<Material>(mat, r));
  
  Sender* s = new Sender(tc.get(), true);
  Bid<Material>::Ptr bid(new Bid<Material>(req, mat, s));
  
  Trade<Material> trade(req, bid, mat->quantity());
  EXPECT_THROW(cyclus::ExecuteTrade(trade), cyclus::ValueError);
  delete s;
  delete r;
}
