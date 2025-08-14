#ifndef CYCLUS_TESTS_SELF_TRADING_TEST_FACILITY_H_
#define CYCLUS_TESTS_SELF_TRADING_TEST_FACILITY_H_

#include "agent.h"
#include "bid_portfolio.h"
#include "context.h"
#include "material.h"
#include "request_portfolio.h"
#include "resource_helpers.h"
#include "test_agents/test_facility.h"
#include "trade.h"
#include "pyne.h"

namespace cyclus {

// Simple test facility for self-trading tests
class SelfTradingTestFacility : public TestFacility {
 public:
  SelfTradingTestFacility(Context* ctx) : TestFacility(ctx) {}
  virtual ~SelfTradingTestFacility() {}

  virtual Agent* Clone() { 
    return new SelfTradingTestFacility(context()); 
  }

  double trade_amt = 100;
  // Simple implementation to support trading
  virtual std::set<RequestPortfolio<Material>::Ptr>
      GetMatlRequests() {
    std::set<RequestPortfolio<Material>::Ptr> ports;
    RequestPortfolio<Material>::Ptr port(
        new RequestPortfolio<Material>());
    
    Material::Ptr mat = NewBlankMaterial(trade_amt);
    port->AddRequest(mat, this, "NaturalUranium");
    ports.insert(port);
    return ports;
  }

  virtual std::set<BidPortfolio<Material>::Ptr>
      GetMatlBids(CommodMap<Material>::type& commod_requests) {
    std::set<BidPortfolio<Material>::Ptr> ports;
    
    if (commod_requests.count("NaturalUranium") > 0) {
      BidPortfolio<Material>::Ptr port(
          new BidPortfolio<Material>());
      
      std::vector<Request<Material>*>& requests = 
          commod_requests.at("NaturalUranium");
      
      for (std::vector<Request<Material>*>::iterator it = 
               requests.begin(); it != requests.end(); ++it) {
        Request<Material>* req = *it;
        Material::Ptr offer = NewBlankMaterial(trade_amt);
        port->AddBid(req, offer, this);
      }
      ports.insert(port);
    }
    return ports;
  }

  virtual void GetMatlTrades(
      const std::vector<Trade<Material>>& trades,
      std::vector<std::pair<Trade<Material>, 
                           Material::Ptr>>& responses) {
    for (std::vector<Trade<Material>>::const_iterator it = 
             trades.begin(); it != trades.end(); ++it) {
      Material::Ptr response = NewBlankMaterial(it->amt);
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
