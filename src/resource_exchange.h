
#ifndef CYCLUS_RESOURCE_EXCHANGE_H_
#define CYCLUS_RESOURCE_EXCHANGE_H_

#include <algorithm>
#include <functional>
#include <set>
                   
#include "bid_portfolio.h"
#include "context.h"
#include "exchange_context.h"
#include "generic_resource.h"
#include "material.h"
#include "request_portfolio.h"
#include "trader.h"
#include "trader_management.h"

namespace cyclus {

/// @brief Preference adjustment method helpers to convert from templates to the
/// Model inheritance hierarchy
template<class T>
inline static void AdjustPrefs(Model* m, typename PrefMap<T>::type& prefs) { }
inline static void AdjustPrefs(Model* m,
                               PrefMap<Material>::type& prefs) {
  m->AdjustMatlPrefs(prefs);
}
inline static void AdjustPrefs(Model* m,
                               typename PrefMap<GenericResource>::type& prefs) {
  m->AdjustGenRsrcPrefs(prefs);
}

/// @class ResourceExchange
///
/// The ResourceExchange class manages the communication for the supply and
/// demand of resources in a simulation. At any given timestep, there are three
/// main phases involved:
/// -# Request for Bids
///     Agents that demand resources of a given type post their\n
///     demands to the exchange
/// -# Response to Request for Bids
///     Agents that supply resources of a given type respond to\n
///     those requests
/// -# Preference Adjustment
///     Preferences for each request-bid pair are set, informing\n
///     the evenutal soluation mechanism
///
/// For example, assuming a simulation Context, ctx, and resource type,
/// ResourceType:
///
/// @code
/// ResourceExchange<ResourceType> exchng(ctx);
/// exchng.AddAllRequests();
/// exchng.AddAllBids();
/// exchng.DoAllAdjustments();
/// @endcode
template <class T>
class ResourceExchange {
 public:
  /// @brief default constructor
  ///
  /// @param ctx the simulation context
  ResourceExchange(Context* ctx) {
    ctx_ = ctx;    
  };

  inline ExchangeContext<T>& ex_ctx() {
    return ex_ctx_;
  } 
  
  /// @brief queries traders and collects all requests for bids
  void AddAllRequests() {
    std::set<Trader*> traders = ctx_->traders();
    std::for_each(
        traders.begin(),
        traders.end(),
        std::bind1st(std::mem_fun(&cyclus::ResourceExchange<T>::__AddRequests),
                     this));
  }
  
  /// @brief queries traders and collects all responses to requests for bids
  void AddAllBids() {
    std::set<Trader*> traders = ctx_->traders();
    std::for_each(
        traders.begin(),
        traders.end(),
        std::bind1st(std::mem_fun(&cyclus::ResourceExchange<T>::__AddBids),
                     this));
  }

  /// @brief adjust preferences for requests given bid responses
  void DoAllAdjustments() {
    std::set<Trader*> traders = ex_ctx_.requesters();
    std::for_each(
        traders.begin(),
        traders.end(),
        std::bind1st(
            std::mem_fun(&cyclus::ResourceExchange<T>::__DoAdjustment),
            this));
  }
  
  /* -------------------- private methods and members ----------------------- */
  Context* ctx_;
  ExchangeContext<T> ex_ctx_;

  /// @brief queries a given facility model for 
  void __AddRequests(Trader* t) {
    std::set< RequestPortfolio<T> > rp = QueryRequests<T>(t);
    typename std::set< RequestPortfolio<T> >::iterator it;
    for (it = rp.begin(); it != rp.end(); ++it) {
      ex_ctx_.AddRequestPortfolio(*it);
    }
  };

  /// @brief queries a given facility model for 
  void __AddBids(Trader* t) {
    std::set< BidPortfolio<T> > bp = QueryBids<T>(t, &ex_ctx_);
    typename std::set< BidPortfolio<T> >::iterator it;
    for (it = bp.begin(); it != bp.end(); ++it) {
      ex_ctx_.AddBidPortfolio(*it);
    }
  };
  
  /// @brief allows a trader and its parents to adjust any preferences in the
  /// system
  void __DoAdjustment(Trader* t) {
    typename PrefMap<T>::type& prefs = ex_ctx_.Prefs(t);
    Model* m = t;
    while (m != NULL) {
      AdjustPrefs(m, prefs);
      m = m->parent();
    }
  };

};

} // namespace cyclus

#endif
