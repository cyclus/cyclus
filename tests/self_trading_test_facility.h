#ifndef CYCLUS_TESTS_SELF_TRADING_TEST_FACILITY_H_
#define CYCLUS_TESTS_SELF_TRADING_TEST_FACILITY_H_

#include "agent.h"
#include "bid_portfolio.h"
#include "context.h"
#include "material.h"
#include "request_portfolio.h"
#include "resource_helpers.h"
#include "test_agents/test_facility.h"
#include "test_trader.h"
#include "trade.h"
#include "pyne.h"

namespace cyclus {

// Simple test facility for self-trading tests
class SelfTradingTestFacility : public TestFacility {
 public:
  SelfTradingTestFacility(Context* ctx, TestObjFactory* fac = NULL) 
      : obj_fac(fac), TestFacility(ctx) {}
  virtual ~SelfTradingTestFacility() {}

  virtual Agent* Clone() { 
    SelfTradingTestFacility* m = new SelfTradingTestFacility(context());
    m->InitFrom(this);
    return m;
  }

  void InitFrom(SelfTradingTestFacility* m) {
    TestFacility::InitFrom(m);
    obj_fac = m->obj_fac;
    context()->RegisterTimeListener(this);
  }

  TestObjFactory* obj_fac;
  // Simple implementation to support trading
  virtual std::set<RequestPortfolio<Material>::Ptr>
      GetMatlRequests() {
    std::set<RequestPortfolio<Material>::Ptr> ports;
    if (obj_fac == NULL) {
      return ports;
    }
    
    RequestPortfolio<Material>::Ptr port(
        new RequestPortfolio<Material>());
    
    port->AddRequest(obj_fac->mat, this, obj_fac->commod);
    ports.insert(port);
    return ports;
  }

  virtual std::set<BidPortfolio<Material>::Ptr>
      GetMatlBids(CommodMap<Material>::type& commod_requests) {
    std::set<BidPortfolio<Material>::Ptr> ports;
    
    if (obj_fac == NULL || commod_requests.count(obj_fac->commod) == 0) {
      return ports;
    }
    
    BidPortfolio<Material>::Ptr port(
        new BidPortfolio<Material>());
    
    std::vector<Request<Material>*>& requests = 
        commod_requests.at(obj_fac->commod);
    
    for (std::vector<Request<Material>*>::iterator it = 
             requests.begin(); it != requests.end(); ++it) {
      Request<Material>* req = *it;
      port->AddBid(req, obj_fac->mat, this);
    }
    ports.insert(port);
    return ports;
  }

  virtual void GetMatlTrades(
      const std::vector<Trade<Material>>& trades,
      std::vector<std::pair<Trade<Material>, 
                           Material::Ptr>>& responses) {
    for (std::vector<Trade<Material>>::const_iterator it = 
             trades.begin(); it != trades.end(); ++it) {
      Material::Ptr response = (obj_fac != NULL) ? obj_fac->mat : NewBlankMaterial(it->amt);
      responses.push_back(std::make_pair(*it, response));
    }
  }

  virtual void AcceptMatlTrades(
      const std::vector<std::pair<Trade<Material>,
                                 Material::Ptr>>& responses) {
    // Note: since we're just testing that the warning is issued,
    // we don't need to do anything here.
  }
};

}  // namespace cyclus

#endif  // CYCLUS_TESTS_SELF_TRADING_TEST_FACILITY_H_
