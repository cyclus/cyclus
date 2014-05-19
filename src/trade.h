#ifndef CYCLUS_SRC_TRADE_H_
#define CYCLUS_SRC_TRADE_H_

#include "bid.h"
#include "request.h"

namespace cyclus {

/// @class Trade
///
/// @brief A Trade is a simple container that associates a request for a
/// resource with a bid for that resource. Additionally, a quantity is assigned
/// to the Trade which may be less than either the request or bid
/// quantity. Finally, Trades have a price member which is not currently used.
template <class T>
struct Trade {
  Request<T>* request;
  Bid<T>* bid;
  double amt;
  double price;

  Trade() : amt(0), price(0) {}

  Trade(Request<T>* request, Bid<T>* bid, double amt)
      : request(request),
        bid(bid),
        amt(amt),
        price(0) {}
};

/// @brief Trade-Trade equality operator
template<class T>
bool operator==(const cyclus::Trade<T>& lhs, const cyclus::Trade<T>& rhs) {
  return  ((lhs.request == rhs.request) &&
           (lhs.bid == rhs.bid) &&
           (lhs.price == rhs.price) &&
           (lhs.amt == rhs.amt));
}

}  // namespace cyclus

#endif  // CYCLUS_SRC_TRADE_H_
