#ifndef CYCLUS_TRADER_H_
#define CYCLUS_TRADER_H_

#include <set>

#include "bid_portfolio.h"
#include "generic_resource.h"
#include "material.h"
#include "composition.h"
#include "request_portfolio.h"
#include "trade.h"
/// #include "model.h"
/// #include "context.h"

namespace cyclus {

template <class T> class ExchangeContext;

/// @class Trader
///
/// @brief A simple API for agents that wish to exchange resources in the
/// simulation
///
/// A Trader is a mixin class designed for agents that wish to exchange
/// resources. It defines the API for the querying of requests, offers, and the
/// corresponding exchanges.
/*
class Trader : virtual public Model {
 public:
  
  Trader(Context* ctx) : Model(ctx) { };
*/
class Trader {
 public:
  /// @brief default implementation for material requests
  virtual std::set< RequestPortfolio<Material> > AddMatlRequests() {
    return std::set< RequestPortfolio<Material> >();
  }
  
  /// @brief default implementation for generic resource requests
  virtual std::set< RequestPortfolio<GenericResource> > AddGenRsrcRequests() {
    return std::set< RequestPortfolio<GenericResource> >();
  }

  /// @brief default implementation for material requests
  virtual std::set< BidPortfolio<Material> >
      AddMatlBids(ExchangeContext<Material>* ec) {
    return std::set< BidPortfolio<Material> >();
  }
  
  /// @brief default implementation for generic resource requests
  virtual std::set< BidPortfolio<GenericResource> >
      AddGenRsrcBids(ExchangeContext<GenericResource>* ec) {
    return std::set< BidPortfolio<GenericResource> >();
  }

  /// @brief default implementation for material trade offer
  virtual Material::Ptr OfferMatlTrade(const Trade<Material>& trade) {
    CompMap cm;
    return Material::CreateUntracked(0.0, Composition::CreateFromMass(cm));
  }

  /// @brief default implementation for generic resource trade offer
  virtual GenericResource::Ptr
      OfferGenRsrcTrade(const Trade<GenericResource>& trade) {
    return GenericResource::CreateUntracked(0.0, "", "");
  }
  
  /// @brief default implementation for material trade acceptance
  virtual void AcceptMatlTrade(const Trade<Material>& trade, Material::Ptr) { }

  /// @brief default implementation for generic resource trade acceptance
  virtual void AcceptGenRsrcTrade(const Trade<GenericResource>& trade,
                                  GenericResource::Ptr) { }
};

} // namespace cyclus

#endif // ifndef CYCLUS_TRADER_H_
