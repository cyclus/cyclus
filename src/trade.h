#ifndef CYCLUS_TRADE_H_
#define CYCLUS_TRADE_H_

#include <boost/shared_ptr.hpp>

namespace cyclus {

class Trader;
  
template <class T>
struct Trade {
  Trader* sender;
  Trader* receiver;
  boost::shared_ptr<T> item;
};

} // namespace cyclus

#endif // ifndef CYCLUS_TRADE_H_
