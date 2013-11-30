#ifndef CYCLUS_BID_H_
#define CYCLUS_BID_H_

#include <boost/shared_ptr.hpp>

#include "request.h"

namespace cyclus {

template<class T> class BidPortfolio;
class Trader;
  
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
      bidder_(bidder),
      id_(next_id_++) {};
  
  /// @return the request being responded to
  inline typename Request<T>::Ptr request() const {
    return request_;
  }

  /// @return the bid object for the request
  inline boost::shared_ptr<T> offer() const {
    return offer_;
  }

  /// @return the model responding the request
  inline Trader* bidder() const {
    return bidder_;
  }
  
  /// @return the portfolio of which this bid is a part
  inline typename BidPortfolio<T>::Ptr portfolio() {
    return portfolio_;
  }

  /// @brief set the portfolio for this bid
  inline void set_portfolio(typename BidPortfolio<T>::Ptr portfolio) {
    portfolio_ = portfolio;
  }
  
  /// @return a unique id for the bid
  inline int id() const {
    return id_;
  }

 private:
  typename Request<T>::Ptr request_;
  boost::shared_ptr<T> offer_;
  Trader* bidder_;
  typename BidPortfolio<T>::Ptr portfolio_;
  int id_;
  static int next_id_;
};

template<class T> int Bid<T>::next_id_ = 0;

/// @brief Bid-Bid equality operator
template<class T>
inline bool operator==(const cyclus::Bid<T>& lhs,
                       const cyclus::Bid<T>& rhs) {
  return  (lhs.request() == rhs.request() &&
           lhs.offer() == rhs.offer() &&
           lhs.portfolio() == rhs.portfolio() &&
           lhs.bidder() == rhs.bidder());
}

/// @brief Bid-Bid comparison operator, allows usage in ordered containers
template<class T>
inline bool operator<(const cyclus::Bid<T>& lhs,
                      const cyclus::Bid<T>& rhs) {
  return  (lhs.id() < rhs.id());
};

} // namespace cyclus

#endif
