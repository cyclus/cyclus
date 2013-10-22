#ifndef CYCLUS_EXCHANGE_AGGREGATOR_H_
#define CYCLUS_EXCHANGE_AGGREGATOR_H_

#include <string>
#include <set>
#include <map>
#include <vector>

#include "bid.h"
#include "bid_portfolio.h"
#include "request.h"
#include "request_portfolio.h"

namespace cyclus {

using std::set;
using std::string;
using std::map;
using std::vector;

/// @class ExchangeAggregator
///
/// @brief The aggregator is designed to provide an ease-of-use interface for
/// querying and reaggregating information regarding requests and bids of a
/// resource exchange.
///
/// The ExchangeAggregator is used by a ResourceExchange or related class to
/// provide introspection into the requests and bids it collects. Specifically,
/// this class is designed to assit in phases of the Dynamic Resource
/// Exchange. The second phase, Respose to Request for Bids, is assisted by
/// grouping requests by commodity type. The third phase, preference adjustment,
/// is assisted by grouping bids by the requester being responded to.
template <class T>
class ExchangeAggregator {
 public:
  /// @brief set the requests to be aggregated
  void set_requests(set< RequestPortfolio<T> >* r) {
    requests_ = r;
    requests_by_commod_ =
        map< string, vector< const
        Request<T>* > >(); // reset mapping
    MapRequestsToCommods();
  }

  /// @return the requests associated with this aggregator
  const set< RequestPortfolio<T> >& requests() const {return *requests_;}

  /// @return a map of commodities to requests for those commodities
  const map< string, vector< const Request<T>* > >& RequestsByCommod() const {
    return requests_by_commod_;
  }

  /// @return the set of requests associated with a commodity
  /// @param c the commodity
  const vector< const Request<T>* >& RequestsForCommod(std::string c) const {
    return requests_by_commod_.at(c);
  }

  /// @brief set the bids to be aggregated
  void set_bids(set< BidPortfolio<T> >* b) {bids_ = b;}
  
  /// @return the bids associated with this aggregator
  const set< BidPortfolio<T> >& bids() const {return *bids_;}
  
 private:
  /// a reference to an exchange's set of requests
  set< RequestPortfolio<T> >* requests_;
 
  /// a reference to an exchange's set of bids
  set< BidPortfolio<T> >* bids_;

  /// mapping commodities to requests
  map< string, vector< const Request<T>* > > requests_by_commod_;

  /// @brief do the work to map commodities
  void MapRequestsToCommods() {
    typename set< RequestPortfolio<T> >::const_iterator p_it;
    for (p_it = requests_->begin(); p_it != requests_->end(); ++p_it) {
      const vector< Request<T> >& r = p_it->requests();
      typename vector< Request<T> >::const_iterator r_it;
      for (r_it = r.begin(); r_it != r.end(); ++r_it) {
        const Request<T>* pr = &(*(r_it));
        requests_by_commod_[r_it->commodity].push_back(pr);
      }
    }
  }
};

} // namespace cyclus

#endif // #ifndef CYCLUS_EXCHANGE_AGGREGATOR_H_
