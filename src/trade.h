#ifndef CYCLUS_TRADE_H_
#define CYCLUS_TRADE_H_

#include "bid.h"
#include "request.h"

namespace cyclus {

template <class T>
struct Trade {
  typename Request<T>::Ptr request;
  typename Bid<T>::Ptr bid;
  double amt;

  Trade() {};
  
  Trade(typename Request<T>::Ptr request, typename Bid<T>::Ptr bid, double amt)
    : request(request),
      bid(bid),
      amt(amt) { };
};

/// @brief equality operator
template<class T>
bool operator==(const cyclus::Trade<T>& lhs,
                const cyclus::Trade<T>& rhs) {
  return  ((lhs.request == rhs.request) &&
           (lhs.bid == rhs.bid) &&
           (lhs.amt == rhs.amt));
}


} // namespace cyclus

#endif // ifndef CYCLUS_TRADE_H_
