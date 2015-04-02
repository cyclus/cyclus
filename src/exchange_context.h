#ifndef CYCLUS_SRC_EXCHANGE_CONTEXT_H_
#define CYCLUS_SRC_EXCHANGE_CONTEXT_H_

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

/// @brief A notion of the point of view from which an trade-related action is
/// being requested
enum TradeSense {
  REQUEST = 0, ///< The action is from the request-perspective 
  BID, ///< The action is from the bid-perspective
  END
};    

template <class T>
struct PrefMap {
  typedef std::map<Request<T>*, std::map<Bid<T>*, double> > type;
};

template <class T>
struct CommodMap {
  typedef std::map<std::string, std::vector<Request<T>*> > type;
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
template <class T>
struct ExchangeContext {
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

  /// @brief adds a bid to the appropriate containers, default trade preference
  /// between request and bid is set
  /// @param pb the bid
  void AddBid(Bid<T>* pb) {
    assert(pb->bidder() != NULL);
    bidders.insert(pb->bidder());

    bids_by_request[pb->request()].push_back(pb);

    trader_prefs[pb->request()->requester()][pb->request()].insert(
        std::make_pair(pb, pb->request()->preference()));
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
  std::map< Request<T>*, std::vector<Bid<T>*> >
      bids_by_request;

  /// @brief maps commodity name to requests for that commodity
  std::map<Trader*, typename PrefMap<T>::type> trader_prefs;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_EXCHANGE_CONTEXT_H_
