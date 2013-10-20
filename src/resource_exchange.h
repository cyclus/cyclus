
#ifndef CYCLUS_RESOURCE_EXCHANGE_H_
#define CYCLUS_RESOURCE_EXCHANGE_H_

#include <vector>
#include <set>
#include <algorithm>

#include "context.h"
#include "generic_resource.h"
#include "material.h"
#include "request_portfolio.h"

namespace cyclus {

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
    std::vector<FacilityModel*> facs = ctx_->facs();
    std::for_each(facs.begin(), facs.end(), AddRequest);
  }

  /// @brief queries a given facility model for 
  void AddRequest(FacilityModel* f) {
    std::set< RequestPortfolio<T> > r = QueryRequest<T>(f);
    requests.insert(r.begin(), r.end());
  };
  
  /// @brief the set of request porfolios
  std::set< RequestPortfolio<T> > requests;
  
 private:
  Context* ctx_;
};

template<> std::set< RequestPortfolio<Material> >
    QueryRequest<Material>(FacilityModel* f) {
  return f->AddMatlRequests();
}

template<> std::set< RequestPortfolio<GenericResource> >
    QueryRequest<GenericResource>(FacilityModel* f) {
  return f->AddGenRsrcRequests();
}

} // namespace cyclus

#endif

