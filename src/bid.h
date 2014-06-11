#ifndef CYCLUS_SRC_BID_H_
#define CYCLUS_SRC_BID_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "request.h"

namespace cyclus {

class Trader;
template<class T> class BidPortfolio;

/// @class Bid
///
/// @brief A Bid encapsulates all the information required to communicate a bid
/// response to a request for a resource, including the resource bid and the
/// bidder.
template <class T>
class Bid {
 public:
  /// @brief a factory method for a bid
  /// @param request the request being responded to by this bid
  /// @param offer the resource being offered in response to the request
  /// @param bidder the bidder
  /// @param portfolio the porftolio of which this bid is a part
  inline static Bid<T>* Create(
      Request<T>* request,
      boost::shared_ptr<T> offer,
      Trader* bidder,
      typename BidPortfolio<T>::Ptr portfolio,
      bool exclusive = false) {
    return new Bid<T>(request, offer, bidder, portfolio, exclusive);
  }

  /// @brief a factory method for a bid for a bid without a portfolio
  /// @warning this factory should generally only be used for testing
  inline static Bid<T>* Create(
      Request<T>* request,
      boost::shared_ptr<T> offer,
      Trader* bidder,
      bool exclusive = false) {
    return new Bid<T>(request, offer, bidder, exclusive);
  }

  /// @return the request being responded to
  inline Request<T>* request() const {
    return request_;
  }

  /// @return the bid object for the request
  inline boost::shared_ptr<T> offer() const {
    return offer_;
  }

  /// @return the agent responding the request
  inline Trader* bidder() const {
    return bidder_;
  }

  /// @return the portfolio of which this bid is a part
  inline typename BidPortfolio<T>::Ptr portfolio() {
    return portfolio_.lock();
  }

  /// @return whether or not this an exclusive bid
  inline bool exclusive() const {
    return exclusive_;
  }

 private:
  /// @brief constructors are private to require use of factory methods
  Bid(Request<T>* request,
      boost::shared_ptr<T> offer,
      Trader* bidder,
      bool exclusive = false)
      : request_(request),
        offer_(offer),
        bidder_(bidder),
        exclusive_(exclusive) {}

  Bid(Request<T>* request,
      boost::shared_ptr<T> offer,
      Trader* bidder,
      typename BidPortfolio<T>::Ptr portfolio,
      bool exclusive = false)
      : request_(request),
        offer_(offer),
        bidder_(bidder),
        portfolio_(portfolio),
        exclusive_(exclusive) {}

  Request<T>* request_;
  boost::shared_ptr<T> offer_;
  Trader* bidder_;
  boost::weak_ptr<BidPortfolio<T> > portfolio_;
  bool exclusive_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_BID_H_
