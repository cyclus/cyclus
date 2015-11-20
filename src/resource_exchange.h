#ifndef CYCLUS_SRC_RESOURCE_EXCHANGE_H_
#define CYCLUS_SRC_RESOURCE_EXCHANGE_H_

#include <algorithm>
#include <functional>
#include <set>

#include "bid_portfolio.h"
#include "context.h"
#include "exchange_context.h"
#include "product.h"
#include "material.h"
#include "request_portfolio.h"
#include "trader.h"
#include "trader_management.h"

namespace cyclus {

/// @brief Preference adjustment method helpers to convert from templates to the
/// Agent inheritance hierarchy
template<class T>
inline static void AdjustPrefs(Agent* m, typename PrefMap<T>::type& prefs) {}
inline static void AdjustPrefs(Agent* m, PrefMap<Material>::type& prefs) {
  m->AdjustMatlPrefs(prefs);
}
inline static void AdjustPrefs(Agent* m, PrefMap<Product>::type& prefs) {
  m->AdjustProductPrefs(prefs);
}
inline static void AdjustPrefs(Trader* t, PrefMap<Material>::type& prefs) {
  t->AdjustMatlPrefs(prefs);
}
inline static void AdjustPrefs(Trader* t, PrefMap<Product>::type& prefs) {
  t->AdjustProductPrefs(prefs);
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
/// exchng.AdjustAll();
/// @endcode
template <class T>
class ResourceExchange {
 public:
  /// @brief default constructor
  ///
  /// @param ctx the simulation context
  ResourceExchange(Context* ctx) {
    sim_ctx_ = ctx;
  }

  inline ExchangeContext<T>& ex_ctx() {
    return ex_ctx_;
  }

  /// @brief queries traders and collects all requests for bids
  void AddAllRequests() {
    InitTraders();
    std::for_each(
        traders_.begin(),
        traders_.end(),
        std::bind1st(std::mem_fun(&cyclus::ResourceExchange<T>::AddRequests_),
                     this));
  }

  /// @brief queries traders and collects all responses to requests for bids
  void AddAllBids() {
    InitTraders();
    std::for_each(
        traders_.begin(),
        traders_.end(),
        std::bind1st(std::mem_fun(&cyclus::ResourceExchange<T>::AddBids_),
                     this));
  }

  /// @brief adjust preferences for requests given bid responses
  void AdjustAll() {
    InitTraders();
    std::set<Trader*> traders = ex_ctx_.requesters;
    std::for_each(
        traders.begin(),
        traders.end(),
        std::bind1st(
            std::mem_fun(&cyclus::ResourceExchange<T>::AdjustPrefs_),
            this));
  }

  /// return true if this is an empty exchange (i.e., no requests exist,
  /// therefore no bids)
  inline bool Empty() { return ex_ctx_.bids_by_request.empty(); }

 private:
  void InitTraders() {
    if (traders_.size() == 0) {
      std::set<Trader*> orig = sim_ctx_->traders();
      std::set<Trader*>::iterator it;
      for (it = orig.begin(); it != orig.end(); ++it) {
        traders_.insert(*it);
      }
    }
  }

  /// @brief queries a given facility agent for
  void AddRequests_(Trader* t) {
    std::set<typename RequestPortfolio<T>::Ptr> rp = QueryRequests<T>(t);
    typename std::set<typename RequestPortfolio<T>::Ptr>::iterator it;
    for (it = rp.begin(); it != rp.end(); ++it) {
      ex_ctx_.AddRequestPortfolio(*it);
    }
  }

  /// @brief queries a given facility agent for
  void AddBids_(Trader* t) {
    std::set<typename BidPortfolio<T>::Ptr> bp =
        QueryBids<T>(t, ex_ctx_.commod_requests);
    typename std::set<typename BidPortfolio<T>::Ptr>::iterator it;
    for (it = bp.begin(); it != bp.end(); ++it) {
      ex_ctx_.AddBidPortfolio(*it);
    }
  }

  /// @brief allows a trader and its parents to adjust any preferences in the
  /// system
  void AdjustPrefs_(Trader* t) {
    typename PrefMap<T>::type& prefs = ex_ctx_.trader_prefs[t];
    AdjustPrefs(t, prefs);
    Agent* m = t->manager()->parent();
    while (m != NULL) {
      AdjustPrefs(m, prefs);
      m = m->parent();
    }
  }

  struct trader_compare {
    bool operator()(Trader* lhs, Trader* rhs) const {
      int left = lhs->manager()->id();
      int right = rhs->manager()->id();
      if (left != right) {
        return left < right;
      } else {
        return lhs < rhs;
      }
    }
  };

  // this sorts traders (and results in iteration...) based on traders'
  // manager id.  Iterating over traders in this order helps increase the
  // determinism of Cyclus overall.  This allows all traders' resource
  // exchange functions are called in a much closer to deterministic order.
  std::set<Trader*, trader_compare> traders_;

  Context* sim_ctx_;
  ExchangeContext<T> ex_ctx_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_RESOURCE_EXCHANGE_H_
