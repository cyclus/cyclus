#ifndef CYCLUS_TESTS_FULL_SIM_TESTS_H_
#define CYCLUS_TESTS_FULL_SIM_TESTS_H_

#include "mock_facility.h"
#include "context.h"
#include "trade.h"
#include "material.h"
#include "model.h"
#include "request_portfolio.h"
#include "bid_portfolio.h"
#include "resource_helpers.h"

namespace cyclus {

class TestObjFactory {
 public:

  TestObjFactory() {
    mat = test_helpers::get_mat();
    commod = "commod";
  }

  Material::Ptr mat;
  std::string commod;
};
  
class TestTrader : public MockFacility {
 public:
 TestTrader(Context* ctx, TestObjFactory* fac = NULL, bool is_requester = true)
   : obj_fac(fac),
     is_requester(is_requester),
     MockFacility(ctx),
     Model(ctx),
     offer(0),
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
    m->offer = offer;
    m->obj_fac = obj_fac;
    m->is_requester = is_requester;
    context()->RegisterTicker(m);
    return m;
  };

  virtual std::set< RequestPortfolio<Material> >
      AddMatlRequests() {
    requests++;
    if (obj_fac == NULL || !is_requester) {
      return std::set< RequestPortfolio<Material> >();
    } else {
      std::set< RequestPortfolio<Material> > ports;
      RequestPortfolio<Material> port;
      Request<Material>::Ptr request(
          new Request<Material>(obj_fac->mat, this, obj_fac->commod));
      req = request; // exp request
      port.AddRequest(request);
      ports.insert(port);
      return ports;
    }
  }
  
  virtual std::set< BidPortfolio<Material> >
      AddMatlBids(ExchangeContext<Material>* ec) {
    bids++;
    if (obj_fac == NULL || is_requester) {
      return std::set< BidPortfolio<Material> >();
    } else {
      req = ec->RequestsForCommod(obj_fac->commod)[0]; // obs request
    
      std::set< BidPortfolio<Material> > ports;
      BidPortfolio<Material> port;
      Bid<Material>::Ptr to_bid(
          new Bid<Material>(req, obj_fac->mat, this));
      bid = to_bid; // exp bid
      port.AddBid(to_bid);
      ports.insert(port);
      return ports;
    }
  }

  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs) {
    bid = prefs[req][0].first; // obs bid
    adjusts++;
  };

  virtual Material::Ptr OfferMatlTrade(const Trade<Material>& trade) {
    offer++;
    obs_trade = Trade<Material>(trade); // obs trade
    return obj_fac->mat;
  }

  virtual void AcceptMatlTrade(const Trade<Material>& trade,
                               Material::Ptr rsrc) {
    obs_trade = Trade<Material>(trade); // obs trade
    mat = rsrc; // obs resource
    accept++;
  }

  TestObjFactory* obj_fac;
  Request<Material>::Ptr req; // obs or exp
  Bid<Material>::Ptr bid; // obs or exp
  Trade<Material> obs_trade; // obs trade
  Material::Ptr mat; // obs mat
  bool is_requester;
  int accept, offer, requests, bids, adjusts;
};

/* // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* class TestRequester : public TestTrader { */
/*  public: */
/*   TestRequester(Context* ctx, Material::Ptr request) */
/*     : TestTrader(ctx), */
/*       Model(ctx), */
/*       req(request) { } */

/*   virtual Model* Clone() { */
/*     TestRequester* m = new TestRequester(*this); */
/*     m->InitFrom(this); */
/*     m->adjusts = adjusts; */
/*     m->requests = requests; */
/*     m->bids = bids; */
/*     m->accept = accept; */
/*     m->request = request; */
/*     context()->RegisterTicker(m); */
/*     return m; */
/*   }; */

/*   Material::Ptr req; */
/* }; */

/* // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* class TestSupplier : public TestTrader { */
/*  public: */
/*   TestSupplier(Context* ctx, Request<Material<::Ptr request, */
/*                Bid<Material>::Ptr bid) */
/*     : TestTrader(ctx), */
/*       Model(ctx), */
/*       req(request), */
/*       bid(bid) { } */
/* }; */

} // namespace cyclus

#endif // ifndef CYCLUS_TESTS_FULL_SIM_TESTS_H_
