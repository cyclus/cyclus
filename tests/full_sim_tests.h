#include "mock_facility.h"
#include "context.h"
#include "trade.h"
#include "material.h"
#include "model.h"
#include "request_portfolio.h"
#include "bid_portfolio.h"

namespace cyclus {
class TestTrader : public MockFacility {
 public:
  TestTrader(Context* ctx)
    : MockFacility(ctx),
      Model(ctx),
      adjusts(0),
      requests(0),
      bids(0),
      accept(0) { };
  
  virtual Model* Clone() {
    TestTrader* m = new TestTrader(*this);
    m->InitFrom(this);
    m->adjusts = adjusts;
    m->requests = requests;
    m->bids = bids;
    m->accept = accept;
    context()->RegisterTicker(m);
    return m;
  };

  virtual std::set< RequestPortfolio<Material> >
      AddMatlRequests() {
    requests++;
    return std::set< RequestPortfolio<Material> >();
  }
  
  virtual std::set< BidPortfolio<Material> >
      AddMatlBids(ExchangeContext<Material>* ec) {
    bids ++;
    return std::set< BidPortfolio<Material> >();
  }

  virtual void AcceptMatlTrade(const Trade<Material>& trade,
                               Material::Ptr) {
    accept++;
  }

  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs) {
    adjusts++;
  };
  
  int accept, requests, bids, adjusts;
};

/* // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* class TestRequester : public TestTrader { */
/*  public: */
/*   TestRequester(Context* ctx) */
/*     : TestTrader(ctx), */
/*       Model(ctx) { } */
/* }; */

/* // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* class TestSupplier : public TestTrader { */
/*  public: */
/*   TestSupplier(Context* ctx) */
/*     : TestTrader(ctx), */
/*       Model(ctx) { } */
/* }; */
} // namespace cyclus
