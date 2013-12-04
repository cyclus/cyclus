#ifndef CYCLUS_TRADER_H_
#define CYCLUS_TRADER_H_

#include <set>

#include "bid_portfolio.h"
#include "composition.h"
#include "generic_resource.h"
#include "material.h"
#include "request_portfolio.h"
#include "trade.h"

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
class Trader : virtual public Model {
 public:
  Trader(Context* ctx) : Model(ctx) {};

  /// @brief default implementation for material requests
  virtual std::set<RequestPortfolio<Material>::Ptr>
      GetMatlRequests() {
    return std::set<RequestPortfolio<Material>::Ptr>();
  }
  
  /// @brief default implementation for generic resource requests
  virtual std::set<RequestPortfolio<GenericResource>::Ptr>
      GetGenRsrcRequests() {
    return std::set<RequestPortfolio<GenericResource>::Ptr >();
  }

  /// @brief default implementation for material requests
  virtual std::set<BidPortfolio<Material>::Ptr>
      GetMatlBids(ExchangeContext<Material>* ec) {
    return std::set<BidPortfolio<Material>::Ptr>();
  }
  
  /// @brief default implementation for generic resource requests
  virtual std::set<BidPortfolio<GenericResource>::Ptr>
      GetGenRsrcBids(ExchangeContext<GenericResource>* ec) {
    return std::set<BidPortfolio<GenericResource>::Ptr>();
  }

  /// @brief default implementation for responding to material trades
  /// @param trades all trades in which this trader is the supplier
  /// @param responses a container to populate with responses to each trade
  virtual void GetMatlTrades(
    const std::vector< Trade<Material> >& trades,
    std::vector<std::pair<Trade<Material>, Material::Ptr> >& responses) {}
  
  /// @brief default implementation for responding to generic resource trades
  /// @param trades all trades in which this trader is the supplier
  /// @param responses a container to populate with responses to each trade
  virtual void GetGenRsrcTrades(
    const std::vector< Trade<GenericResource> >& trades,
    std::vector<std::pair<Trade<GenericResource>,
    GenericResource::Ptr> >& responses) {}

  /// @brief default implementation for material trade acceptance
  virtual void AcceptMatlTrades(
      const std::vector<std::pair<Trade<Material>,
      Material::Ptr> >& responses) {}

  /// @brief default implementation for generic resource trade acceptance
  virtual void AcceptGenRsrcTrades(
      const std::vector<std::pair<Trade<GenericResource>,
      GenericResource::Ptr> >& responses) {}
};

} // namespace cyclus

#endif // ifndef CYCLUS_TRADER_H_
