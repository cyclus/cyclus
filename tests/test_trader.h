#ifndef CYCLUS_TESTS_TEST_TRADER_H_
#define CYCLUS_TESTS_TEST_TRADER_H_

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
     accept(0) {};

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

  virtual std::set<RequestPortfolio<Material>::Ptr>
      AddMatlRequests() {
    requests++;
    if (obj_fac == NULL || !is_requester) {
      return std::set<RequestPortfolio<Material>::Ptr>();
    } else {
      std::set<RequestPortfolio<Material>::Ptr> ports;
      RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
      Request<Material>::Ptr request(
          new Request<Material>(obj_fac->mat, this, obj_fac->commod));
      req = request; // exp request
      port->AddRequest(request);
      ports.insert(port);
      return ports;
    }
  }
  
  virtual std::set<BidPortfolio<Material>::Ptr>
      AddMatlBids(ExchangeContext<Material>* ec) {
    bids++;
    if (obj_fac == NULL || is_requester) {
      return std::set<BidPortfolio<Material>::Ptr>();
    } else {
      req = ec->requests_by_commod[obj_fac->commod][0]; // obs request
    
      std::set<BidPortfolio<Material>::Ptr> ports;
      BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());
      Bid<Material>::Ptr to_bid(
          new Bid<Material>(req, obj_fac->mat, this));
      bid = to_bid; // exp bid
      port->AddBid(to_bid);
      ports.insert(port);
      return ports;
    }
  }

  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs) {
    bid = prefs[req][0].first; // obs bid
    adjusts++;
  };

  virtual void PopulateMatlTradeResponses(
    const std::vector< Trade<Material> >& trades,
    std::vector<std::pair<Trade<Material>, Material::Ptr> >& responses) {
    std::vector< Trade<Material> >::const_iterator it;
    for (it = trades.begin(); it != trades.end(); ++it) {
      obs_trade = Trade<Material>(*it);
      responses.push_back(std::make_pair(*it, obj_fac->mat));
      offer++;
    }
  }
  
  virtual void AcceptMatlTrades(
      const std::vector<std::pair<Trade<Material>,
      Material::Ptr> >& responses) {
    if (responses.size() > 0) {
      obs_trade = Trade<Material>(responses.at(0).first);
      mat = responses.at(0).second;
    }
    accept += responses.size();
  }

  TestObjFactory* obj_fac;
  Request<Material>::Ptr req; // obs or exp
  Bid<Material>::Ptr bid; // obs or exp
  Trade<Material> obs_trade; // obs trade
  Material::Ptr mat; // obs mat
  bool is_requester;
  int accept, offer, requests, bids, adjusts;
};

} // namespace cyclus

#endif // ifndef CYCLUS_TESTS_TEST_TRADER_H_
