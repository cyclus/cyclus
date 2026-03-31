#ifndef CYCLUS_SRC_EXCHANGE_CONTEXT_H_
#define CYCLUS_SRC_EXCHANGE_CONTEXT_H_

#include <assert.h>
#include <map>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include "bid.h"
#include "bid_portfolio.h"
#include "request.h"
#include "request_portfolio.h"

// Undefines isnan from pyne
#ifdef isnan
#undef isnan
#endif

namespace cyclus {

template <class T> struct PrefMap {
  typedef std::map<Request<T>*, std::map<Bid<T>*, double>> type;
  typedef Request<T>* request_ptr;
  typedef Bid<T>* bid_ptr;
};

/// @brief Maps for storing unit cost and unit value adjustments
/// These use the same structure as PrefMap but are semantically different:
/// - UnitCostMap stores unit cost adjustments (from bids)
/// - UnitValueMap stores unit value adjustments (from requests)
template <class T> struct UnitCostMap {
  typedef std::map<Request<T>*, std::map<Bid<T>*, double>> type;
};
template <class T> struct UnitValueMap {
  typedef std::map<Request<T>*, std::map<Bid<T>*, double>> type;
};

template <class T> struct CommodMap {
  typedef std::map<std::string, std::vector<Request<T>*>> type;
  typedef Request<T>* request_ptr;
};

/// @class ExchangeContext
///
/// @brief The ExchangeContext is designed to provide an ease-of-use interface
/// for querying and reaggregating information regarding requests and bids of a
/// resource exchange.
///
/// The ExchangeContext is used by a ResourceExchange or related class to
/// provide introspection into the requests and bids it collects. Specifically,
/// this class is designed to assist in phases of the Dynamic Resource
/// Exchange. The second phase, Response to Request for Bids, is assisted by
/// grouping requests by commodity type. The third phase, preference adjustment,
/// is assisted by grouping bids by the requester being responded to.
template <class T> struct ExchangeContext {
 public:
  /// @brief adds a request to the context
  void AddRequestPortfolio(const typename RequestPortfolio<T>::Ptr port) {
    requests.push_back(port);
    const std::vector<Request<T>*>& vr = port->requests();
    typename std::vector<Request<T>*>::const_iterator it;

    for (it = vr.begin(); it != vr.end(); ++it) {
      Request<T>* pr = *it;
      AddRequest(*it);
    }
  }

  /// @brief Adds an individual request
  void AddRequest(Request<T>* pr) {
    assert(pr->requester() != NULL);
    requesters.insert(pr->requester());
    commod_requests[pr->commodity()].push_back(pr);
  }

  /// @brief adds a bid to the context
  void AddBidPortfolio(const typename BidPortfolio<T>::Ptr port) {
    bids.push_back(port);
    const std::set<Bid<T>*>& vr = port->bids();
    typename std::set<Bid<T>*>::const_iterator it;

    for (it = vr.begin(); it != vr.end(); ++it) {
      Bid<T>* pb = *it;
      AddBid(pb);
    }
  }

  /// @brief adds a bid to the appropriate containers, default unit cost
  /// and unit value are set
  /// @param pb the bid
  void AddBid(Bid<T>* pb) {
    assert(pb->bidder() != NULL);
    bidders.insert(pb->bidder());

    bids_by_request[pb->request()].push_back(pb);

    // unit cost comes from bid preference (or 0.0 if NaN)
    double bid_pref = pb->preference();
    double unit_cost = std::isnan(bid_pref) ? 0.0 : bid_pref;
    
    // unit value comes from request preference
    double req_pref = pb->request()->preference();
    double unit_value = std::isnan(req_pref) ? 0.0 : req_pref;
    
    // Store unit cost and unit value separately
    trader_costs[pb->request()->requester()][pb->request()].insert(
        std::make_pair(pb, unit_cost));
    trader_values[pb->request()->requester()][pb->request()].insert(
        std::make_pair(pb, unit_value));
    
    // Keep trader_prefs for backward compatibility during transition
    trader_prefs[pb->request()->requester()][pb->request()].insert(
        std::make_pair(pb, unit_cost));
  }

  /// @brief a reference to an exchange's set of requests
  std::vector<typename RequestPortfolio<T>::Ptr> requests;

  /// @brief a reference to an exchange's set of bids
  std::vector<typename BidPortfolio<T>::Ptr> bids;

  /// @brief known requesters
  std::set<Trader*> requesters;

  /// @brief known bidders
  std::set<Trader*> bidders;

  /// @brief maps commodity name to requests for that commodity
  typename CommodMap<T>::type commod_requests;

  /// @brief maps request to all bids for request
  std::map<Request<T>*, std::vector<Bid<T>*>> bids_by_request;

  /// @brief maps trader to request to bid to unit cost adjustments
  std::map<Trader*, typename UnitCostMap<T>::type> trader_costs;
  
  /// @brief maps trader to request to bid to unit value adjustments
  std::map<Trader*, typename UnitValueMap<T>::type> trader_values;
  
  /// @deprecated Use trader_costs and trader_values instead. Kept for backward compatibility.
  std::map<Trader*, typename PrefMap<T>::type> trader_prefs;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_EXCHANGE_CONTEXT_H_
