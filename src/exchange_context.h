#ifndef CYCLUS_EXCHANGE_CONTEXT_H_
#define CYCLUS_EXCHANGE_CONTEXT_H_

#include <assert.h>
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

  ExchangeContext() {
    requests_.reserve(10);
  }
  
  ~ExchangeContext() {
    requests_.clear();
    typename map< std::string, std::vector< const Request<T>* > >::iterator it;
    for (it = requests_by_commod_.begin(); it != requests_by_commod_.end(); ++it) {
      it->second.clear();
    }
    requests_by_commod_.clear();
  }
  
  /// @brief adds a request to the context
  void AddRequestPortfolio(const RequestPortfolio<T>& r) {
    int index = requests_.size();
    requests_.push_back(r); // copy portfolio for future references
    //const RequestPortfolio<T>& ours = requests_.back();
    const RequestPortfolio<T>& ours = requests_.at(index);
    const std::vector< Request<T> >& vr = ours.requests();
    typename std::vector< Request<T> >::const_iterator it;
    for (it = vr.begin(); it != vr.end(); ++it) {
      if (requests_by_commod_.count(it->commodity) == 0) {
        requests_by_commod_[it->commodity] = std::vector< const Request<T>* >();
      }
      const Request<T>* pr = &(*it);
      requests_by_commod_[it->commodity].push_back(pr);
    }
  }

  /// @brief 
  inline const vector< RequestPortfolio<T> >& requests() {return requests_;}
  
  /// @brief 
  inline const vector< const Request<T>* >& RequestsForCommod(std::string commod) {
    return requests_by_commod_[commod];
  }
  
 private:
  /// a reference to an exchange's set of requests
  std::vector< RequestPortfolio<T> > requests_;

  /// maps commodity name to requests for that commodity
  map< std::string, std::vector< const Request<T>* > > requests_by_commod_;
};

} // namespace cyclus

#endif // #ifndef CYCLUS_EXCHANGE_CONTEXT_H_
