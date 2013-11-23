#include "mock_facility.h"
#include "context.h"
#include "trade.h"
#include "material.h"
#include "model.h"
#include "request_portfolio.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TestSender : public MockFacility {
 public:
  TestSender(cyclus::Context* ctx, bool adjust = false)
    : MockFacility(ctx),
      cyclus::Model(ctx),
      accept(0),
      requests(0) { };
  
  virtual cyclus::Model* Clone() {
    TestSender* m = new TestSender(*this);
    m->InitFrom(this);
    m->accept = accept;
    m->requests = requests;
    context()->RegisterTicker(m);
    return m;
  };

  virtual std::set< cyclus::RequestPortfolio<cyclus::Material> > AddMatlRequests() {
    requests++;
    return std::set< cyclus::RequestPortfolio<cyclus::Material> >();
  }

  /// @brief default implementation for material trade acceptance
  virtual void AcceptMatlTrade(const cyclus::Trade<cyclus::Material>& trade,
                               cyclus::Material::Ptr) {
    accept++;
  }

  int accept, requests;
};

/// cyclus::Material::Ptr OfferMatlTrade(const cyclus::Trade<cyclus::Material>& trade) {
///     offer++;
///     return mat;
///   }

///   /// @brief default implementation for material requests
///   virtual std::set< BidPortfolio<Material> >
///       AddMatlBids(ExchangeContext<Material>* ec) {
///     return std::set< BidPortfolio<Material> >();
///   }
