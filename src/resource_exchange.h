
#ifndef CYCLUS_RESOURCE_EXCHANGE_H_
#define CYCLUS_RESOURCE_EXCHANGE_H_

#include <set>
#include <algorithm>
#include <functional>
                   
#include "context.h"
#include "trader.h"
#include "generic_resource.h"
#include "material.h"
#include "request_portfolio.h"
#include "bid_portfolio.h"
#include "exchange_context.h"

namespace cyclus {

template<class T> std::set< RequestPortfolio<T> > QueryRequests(Trader* e) {
  return std::set< RequestPortfolio<T> >();
}
  
template<> std::set< RequestPortfolio<Material> >
    QueryRequests<Material>(Trader* e) {
  return e->AddMatlRequests();
}

template<> std::set< RequestPortfolio<GenericResource> >
    QueryRequests<GenericResource>(Trader* e) {
  return e->AddGenRsrcRequests();
}

template<class T> class ExchangeContext;
  
template<class T> std::set< BidPortfolio<T> >
    QueryBids(Trader* e, ExchangeContext<T>* ec) {
  return std::set< BidPortfolio<T> >();
}
  
template<> std::set< BidPortfolio<Material> >
    QueryBids<Material>(Trader* e, ExchangeContext<Material>* ec) {
  return e->AddMatlBids(ec);
}

template<> std::set< BidPortfolio<GenericResource> >
    QueryBids<GenericResource>(Trader* e, ExchangeContext<GenericResource>* ec) {
  return e->AddGenRsrcBids(ec);
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
template <class T>
class ResourceExchange {
 public:
  /// @brief default constructor
  ///
  /// @param ctx the simulation context
  ResourceExchange(Context* ctx) {
    ctx_ = ctx;    
  };

  inline const ExchangeContext<T>& ex_ctx() {return ex_ctx_;} 
  
  /// @brief queries facilities and collects all requests for bids
  void CollectRequests() {
    std::set<Trader*> traders = ctx_->traders();
    std::for_each(
        traders.begin(),
        traders.end(),
        std::bind1st(std::mem_fun(&cyclus::ResourceExchange<T>::AddRequests), this));
  }

  /// @brief queries a given facility model for 
  void AddRequests(Trader* f) {
    std::set< RequestPortfolio<T> > r = QueryRequests<T>(f);
    // // note sure why this doesn't work =(, borks because it can't overload operator()
    // std::for_each(
    //     r.begin(),
    //     r.end(),
    //     std::bind1st(
    //         std::mem_fun(&cyclus::ExchangeContext<T>::AddRequestPortfolio), &ex_ctx_
    //                  )
    //               );
    typename std::set< RequestPortfolio<T> >::iterator it;
    for (it = r.begin(); it != r.end(); ++it) {
      ex_ctx_.AddRequestPortfolio(*it);
    }
  };
  
  /// @brief queries facilities and collects all requests for bids
  void CollectBids() {
    std::set<Trader*> traders = ctx_->traders();
    std::for_each(
        traders.begin(),
        traders.end(),
        std::bind1st(std::mem_fun(&cyclus::ResourceExchange<T>::AddBids), this));
  }

  /// @brief queries a given facility model for 
  void AddBids(Trader* f) {
    std::set< BidPortfolio<T> > r = QueryBids<T>(f, &ex_ctx_);
    typename std::set< BidPortfolio<T> >::iterator it;
    for (it = r.begin(); it != r.end(); ++it) {
      ex_ctx_.AddBidPortfolio(*it);
    }
  };

  /// /// @brief adjust preferences for requests given bid responses
  /// void PrefAdjustment() {
  ///   std::set<Trader*> traders = ctx_->traders();
  ///   std::for_each(
  ///       traders.begin(),
  ///       traders.end(),
  ///       std::bind1st(std::mem_fun(&cyclus::ResourceExchange<T>::AdjustPrefs), this));
  /// }

  /// /// @brief
  /// void AdjustPrefs(Trader* f) {
  ///   std::set< BidPortfolio<T> > r = QueryPrefs<T>(f, this);
  ///   /// bids.insert(r.begin(), r.end());
  /// };
  
 private:
  Context* ctx_;
  ExchangeContext<T> ex_ctx_;
};

} // namespace cyclus

#endif

