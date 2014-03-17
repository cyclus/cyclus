#ifndef CYCLUS_SRC_TRADER_H_
#define CYCLUS_SRC_TRADER_H_

#include <set>

#include "bid_portfolio.h"
#include "composition.h"
#include "exchange_context.h"
#include "product.h"
#include "material.h"
#include "request_portfolio.h"
#include "trade.h"

namespace cyclus {

/// @class Trader
///
/// @brief A simple API for agents that wish to exchange resources in the
/// simulation
///
/// A Trader is a mixin class designed for agents that wish to exchange
/// resources. It defines the API for the querying of requests, offers, and the
/// corresponding exchanges.
class Trader {
 public:
  Trader(Agent* manager) : manager_(manager) {}

  virtual Agent* manager() {
    return manager_;
  }

  /// @brief default implementation for material requests
  virtual std::set<RequestPortfolio<Material>::Ptr>
      GetMatlRequests() {
    return std::set<RequestPortfolio<Material>::Ptr>();
  }

  /// @brief default implementation for product requests
  virtual std::set<RequestPortfolio<Product>::Ptr>
      GetProductRequests() {
    return std::set<RequestPortfolio<Product>::Ptr>();
  }

  /// @brief default implementation for material requests
  virtual std::set<BidPortfolio<Material>::Ptr>
      GetMatlBids(const CommodMap<Material>::type& commod_requests) {
    return std::set<BidPortfolio<Material>::Ptr>();
  }

  /// @brief default implementation for product requests
  virtual std::set<BidPortfolio<Product>::Ptr>
      GetProductBids(const CommodMap<Product>::type&
                     commod_requests) {
    return std::set<BidPortfolio<Product>::Ptr>();
  }

  /// default implementation for material preferences.
  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs) {}

  /// default implementation for material preferences.
  virtual void AdjustProductPrefs(PrefMap<Product>::type& prefs) {}

  /// @brief default implementation for responding to material trades
  /// @param trades all trades in which this trader is the supplier
  /// @param responses a container to populate with responses to each trade
  virtual void GetMatlTrades(
      const std::vector< Trade<Material> >& trades,
      std::vector<std::pair<Trade<Material>, Material::Ptr> >& responses) {}

  /// @brief default implementation for responding to product trades
  /// @param trades all trades in which this trader is the supplier
  /// @param responses a container to populate with responses to each trade
  virtual void GetProductTrades(
      const std::vector< Trade<Product> >& trades,
      std::vector<std::pair<Trade<Product>,
      Product::Ptr> >& responses) {}

  /// @brief default implementation for material trade acceptance
  virtual void AcceptMatlTrades(
      const std::vector<std::pair<Trade<Material>,
      Material::Ptr> >& responses) {}

  /// @brief default implementation for product trade acceptance
  virtual void AcceptProductTrades(
      const std::vector<std::pair<Trade<Product>,
      Product::Ptr> >& responses) {}

 private:
  Agent* manager_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_TRADER_H_
