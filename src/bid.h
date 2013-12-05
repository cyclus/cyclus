#ifndef CYCLUS_BID_H_
#define CYCLUS_BID_H_

#include <boost/shared_ptr.hpp>

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
  typedef boost::shared_ptr< Bid<T> > Ptr;

  /// @brief a factory method for a bid
  /// @param request the request being responded to by this bid
  /// @param offer the resource being offered in response to the request
  /// @param bidder the bidder
  /// @param portfolio the porftolio of which this bid is a part
  inline static typename Bid<T>::Ptr Create(
      typename Request<T>::Ptr request, 
      boost::shared_ptr<T> offer,
      Trader* bidder,
      typename BidPortfolio<T>::Ptr portfolio) {
    return Ptr(new Bid<T>(request, offer, bidder, portfolio));
  }

  /// @brief a factory method for a bid for a bid without a portfolio
  /// @warning this factory should generally only be used for testing
  inline static typename Bid<T>::Ptr Create(typename Request<T>::Ptr request, 
                                            boost::shared_ptr<T> offer,
                                            Trader* bidder) {
    return Ptr(new Bid<T>(request, offer, bidder));
  }
  
  /// @return the request being responded to
  inline typename Request<T>::Ptr request() const { return request_; }

  /// @return the bid object for the request
  inline boost::shared_ptr<T> offer() const { return offer_; }

  /// @return the model responding the request
  inline Trader* bidder() const { return bidder_; }
  
  /// @return the portfolio of which this bid is a part
  inline typename BidPortfolio<T>::Ptr portfolio() { return portfolio_; }

 private:
  /// @brief constructors are private to require use of factory methods
  Bid(typename Request<T>::Ptr request, 
      boost::shared_ptr<T> offer,
      Trader* bidder)
    : request_(request),
      offer_(offer),
      bidder_(bidder) {};

  Bid(typename Request<T>::Ptr request, 
      boost::shared_ptr<T> offer,
      Trader* bidder,
      typename BidPortfolio<T>::Ptr portfolio)
    : request_(request),
      offer_(offer),
      bidder_(bidder),
      portfolio_(portfolio) {};

  typename Request<T>::Ptr request_;
  boost::shared_ptr<T> offer_;
  Trader* bidder_;
  typename BidPortfolio<T>::Ptr portfolio_;
};

} // namespace cyclus

#endif
