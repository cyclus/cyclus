#ifndef CYCLUS_EXCHANGE_CONTEXT_H_
#define CYCLUS_EXCHANGE_CONTEXT_H_

#include <assert.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "bid.h"
#include "bid_portfolio.h"
#include "request.h"
#include "request_portfolio.h"

namespace cyclus {

template <class T>
struct PrefMap {
  typedef std::map< typename Request<T>::Ptr,
    std::vector< std::pair< typename Bid<T>::Ptr, double > > > type;
};

/// @class ExchangeContext
///
/// @brief The context is designed to provide an ease-of-use interface for
/// querying and reaggregating information regarding requests and bids of a
/// resource exchange.
///
/// The ExchangeContext is used by a ResourceExchange or related class to
/// provide introspection into the requests and bids it collects. Specifically,
/// this class is designed to assist in phases of the Dynamic Resource
/// Exchange. The second phase, Respose to Request for Bids, is assisted by
/// grouping requests by commodity type. The third phase, preference adjustment,
/// is assisted by grouping bids by the requester being responded to.
template <class T>
class ExchangeContext {
 public:  
  /// @brief adds a request to the context
  void AddRequestPortfolio(const typename RequestPortfolio<T>::Ptr port) {
    requests_.push_back(port);
    const std::vector<typename Request<T>::Ptr>& vr = port->requests();
    typename std::vector<typename Request<T>::Ptr>::const_iterator it;

    if (!vr.empty()) {
      assert(port->requester() != NULL);
      requesters_.insert(port->requester());
    }
    
    for (it = vr.begin(); it != vr.end(); ++it) {
      typename Request<T>::Ptr pr = *it;
      __AddRequest(*it);
    }
  }

  /// @brief Adds an individual request
  void __AddRequest(typename Request<T>::Ptr pr) {
    requests_by_commod_[pr->commodity()].push_back(pr);
  }
  
  /// @brief adds a bid to the context
  void AddBidPortfolio(const typename BidPortfolio<T>::Ptr port) {
    bids_.push_back(port);
    const std::set<typename Bid<T>::Ptr>& vr = port->bids();
    typename std::set<typename Bid<T>::Ptr>::const_iterator it;
    
    if (!vr.empty()) {
      bidders_.insert(port->bidder());
    }

    for (it = vr.begin(); it != vr.end(); ++it) {
      typename Bid<T>::Ptr pb = *it;
      __AddBid(pb);
    }
  }

  /// @return all known request portfolios
  inline const std::vector<typename RequestPortfolio<T>::Ptr>&
      requests() const {
    return requests_;
  }
  
  /// @return all known requesters
  inline const std::set<Trader*>& requesters() const { return requesters_; }

  /// @return all known bid portfolios
  inline const std::vector<typename BidPortfolio<T>::Ptr>&
      bids() const {
    return bids_;
  }

  /// @return all known bidders
  inline const std::set<Trader*>& bidders() const { return bidders_; }
  
  /// @return all known requests for a given commodity
  /// @param commod the commodity
  inline const std::vector<typename Request<T>::Ptr>&
      RequestsForCommod(std::string commod) const {
    return requests_by_commod_.at(commod);
  }
  inline const std::vector<typename Request<T>::Ptr>&
      RequestsForCommod(std::string commod) {
    return requests_by_commod_[commod];
  }
  
  /// @return all known bids for a request
  /// @param request the request
  inline const std::vector<typename Bid<T>::Ptr>&
      BidsForRequest(typename Request<T>::Ptr request) const {
    return bids_by_request_.at(request);
  }
  inline const std::vector<typename Bid<T>::Ptr>&
      BidsForRequest(typename Request<T>::Ptr request) {
    return bids_by_request_[request];
  }

  /// @return all known preferences for a requester
  /// @param requester the requester
  inline typename PrefMap<T>::type& Prefs(Trader* requester) const {
    return trader_prefs_.at(requester);
  }
  inline typename PrefMap<T>::type& Prefs(Trader* requester) {
    return trader_prefs_[requester];
  }

  /* -------------------- private methods and members ----------------------- */
  /// @brief a reference to an exchange's set of requests
  std::vector<typename RequestPortfolio<T>::Ptr> requests_;

  /// @brief a reference to an exchange's set of bids
  std::vector<typename BidPortfolio<T>::Ptr> bids_;

  /// @brief known requesters
  std::set<Trader*> requesters_;
  
  /// @brief known bidders
  std::set<Trader*> bidders_;
  
  /// @brief maps commodity name to requests for that commodity
  std::map< std::string, std::vector<typename Request<T>::Ptr> >
      requests_by_commod_;

  /// @brief maps request to all bids for request
  std::map< typename Request<T>::Ptr, std::vector<typename Bid<T>::Ptr> >
      bids_by_request_;

  /// @brief maps commodity name to requests for that commodity
  std::map<Trader*, typename PrefMap<T>::type> trader_prefs_;

  /// @brief adds a bid to the appropriate containers
  /// @param pb the bid
  void __AddBid(typename Bid<T>::Ptr pb) {
    bids_by_request_[pb->request()].push_back(pb);

    trader_prefs_[pb->request()->requester()][pb->request()].push_back(
        std::make_pair(pb, pb->request()->preference()));
  } 
};

} // namespace cyclus

#endif // #ifndef CYCLUS_EXCHANGE_CONTEXT_H_
