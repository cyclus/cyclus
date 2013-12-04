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
  
  Bid(typename Request<T>::Ptr request, 
      boost::shared_ptr<T> offer,
      Trader* bidder)
    : request_(request),
      offer_(offer),
      bidder_(bidder){};
  
  /// @return the request being responded to
  inline typename Request<T>::Ptr request() const { return request_; }

  /// @return the bid object for the request
  inline boost::shared_ptr<T> offer() const { return offer_; }

  /// @return the model responding the request
  inline Trader* bidder() const { return bidder_; }
  
  /// @return the portfolio of which this bid is a part
  inline typename BidPortfolio<T>::Ptr portfolio() { return portfolio_; }

  /// @brief set the portfolio for this bid
  inline void set_portfolio(typename BidPortfolio<T>::Ptr portfolio) {
    portfolio_ = portfolio;
  }

 private:
  typename Request<T>::Ptr request_;
  boost::shared_ptr<T> offer_;
  Trader* bidder_;
  typename BidPortfolio<T>::Ptr portfolio_;
};

} // namespace cyclus

#endif
