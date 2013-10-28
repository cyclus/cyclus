#ifndef CYCLUS_EXCHANGE_CONTEXT_H_
#define CYCLUS_EXCHANGE_CONTEXT_H_

#include <assert.h>
#include <string>
#include <map>
#include <vector>

#include "bid.h"
#include "bid_portfolio.h"
#include "request.h"
#include "request_portfolio.h"

namespace cyclus {

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
  void AddRequestPortfolio(const RequestPortfolio<T>& port) {
    requests_.push_back(port);
    const std::vector<typename Request<T>::Ptr>& vr = port.requests();
    typename std::vector<typename Request<T>::Ptr>::const_iterator it;

    if (!vr.empty()) {
      assert(port.requester() != NULL);
      requesters_.insert(port.requester());
    }
    
    for (it = vr.begin(); it != vr.end(); ++it) {
      typename Request<T>::Ptr pr = *it;
      if (requests_by_commod_.count(pr->commodity) == 0) {
        requests_by_commod_[pr->commodity] = std::vector<typename Request<T>::Ptr>();
      }
      requests_by_commod_[pr->commodity].push_back(pr);
    }
  }

  /// @brief adds a bid to the context
  void AddBidPortfolio(const BidPortfolio<T>& port) {
    bids_.push_back(port);
    const std::set<typename Bid<T>::Ptr>& vr = port.bids();
    typename std::set<typename Bid<T>::Ptr>::const_iterator it;
    
    if (!vr.empty()) {
      assert(port.bidder() != NULL);
      bidders_.insert(port.bidder());
    }

    for (it = vr.begin(); it != vr.end(); ++it) {
      typename Bid<T>::Ptr pb = *it;
      if (bids_by_request_.count(pb->request) == 0) {
        bids_by_request_[pb->request] = std::vector<typename Bid<T>::Ptr>();
      }
      bids_by_request_[pb->request].push_back(pb);
    }
  }

  /// @brief 
  inline const std::vector< RequestPortfolio<T> >& requests() const {return requests_;}
  
  /// @brief 
  inline const std::set<const Trader*>& requesters() const {return requesters_;}

  /// @brief 
  inline const std::vector< BidPortfolio<T> >& bids() const {return bids_;}

  /// @brief 
  inline const std::set<const Trader*>& bidders() const {return bidders_;}
  
  /// @brief 
  inline const std::vector< typename Request<T>::Ptr >&
      RequestsForCommod(std::string commod) const {
    return requests_by_commod_.at(commod);
  }
  
  /// @brief 
  inline const std::vector< typename Request<T>::Ptr >&
      RequestsForCommod(std::string commod) {
    return requests_by_commod_[commod];
  }
  
  /// @brief 
  inline const std::vector< typename Bid<T>::Ptr >&
      BidsForRequest(typename Request<T>::Ptr request) const {
    return bids_by_request_.at(request);
  }
  
  /// @brief 
  inline const std::vector< typename Bid<T>::Ptr >&
      BidsForRequest(typename Request<T>::Ptr request) {
    return bids_by_request_[request];
  }
  
 private:
  /// a reference to an exchange's set of requests
  std::vector< RequestPortfolio<T> > requests_;

  /// a reference to an exchange's set of bids
  std::vector< BidPortfolio<T> > bids_;

  /// known requesters
  std::set<const Trader*> requesters_;
  
  /// known bidders
  std::set<const Trader*> bidders_;
  
  /// maps commodity name to requests for that commodity
  std::map< std::string, std::vector<typename Request<T>::Ptr> > requests_by_commod_;

  /// maps request to all bids for request
  std::map< typename Request<T>::Ptr, std::vector<typename Bid<T>::Ptr> >
      bids_by_request_;
};

} // namespace cyclus

#endif // #ifndef CYCLUS_EXCHANGE_CONTEXT_H_
