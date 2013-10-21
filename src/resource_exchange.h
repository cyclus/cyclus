
#ifndef CYCLUS_RESOURCE_EXCHANGE_H_
#define CYCLUS_RESOURCE_EXCHANGE_H_

#include <set>
#include <algorithm>
#include <functional>
                   
#include "context.h"
#include "exchanger.h"
#include "generic_resource.h"
#include "material.h"
#include "request_portfolio.h"

namespace cyclus {

template<class T> std::set< RequestPortfolio<T> > QueryRequest(Exchanger* f) {
  return std::set< RequestPortfolio<T> >();
}
  
template<> std::set< RequestPortfolio<Material> >
    QueryRequest<Material>(Exchanger* f) {
  return f->AddMatlRequests();
}

template<> std::set< RequestPortfolio<GenericResource> >
    QueryRequest<GenericResource>(Exchanger* f) {
  return f->AddGenRsrcRequests();
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

  /// @brief queries facilities and collects all requests for bids
  void CollectRequests() {
    std::set<Exchanger*> exchangers = ctx_->exchangers();
    std::for_each(
        exchangers.begin(),
        exchangers.end(),
        std::bind1st(std::mem_fun(&cyclus::ResourceExchange<T>::AddRequest), this));
  }

  /// @brief queries a given facility model for 
  void AddRequest(Exchanger* f) {
    std::set< RequestPortfolio<T> > r = QueryRequest<T>(f);
    requests.insert(r.begin(), r.end());
  };
  
  /// @brief the set of request porfolios
  std::set< RequestPortfolio<T> > requests;
  
 private:
  Context* ctx_;
};

} // namespace cyclus

#endif

