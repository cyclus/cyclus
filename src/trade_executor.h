#ifndef CYCLUS_TRADE_EXECUTOR_H_
#define CYCLUS_TRADE_EXECUTOR_H_

#include "trade.h"
#include "trader.h"

namespace cyclus {

template <class T>
class TradeExecutor {
 public:
  explicit TradeExecutor(const std::vector< Trade<T> >& trades)
    : trades_(trades) { };

  void ExecuteTrades() {
    __GroupTradesBySupplier();
  };
  
  void RecordTrades() {
  };
  
  /* -------------------- private methods and members -------------------------- */
  const std::vector< Trade<T> >& trades_;

  // map of supplier to container of all trades associated with that supplier
  std::map<Trader*,
      std::vector< Trade<T> > > trades_by_supplier_;
  
  std::map<Trader*,
      std::vector< std::pair<Trade<T>, typename T::Ptr> > >
      trades_by_requester_;
  
  std::map<std::pair<Trader*, Trader*>,
      std::vector< std::pair<Trade<T>, typename T::Ptr> > > all_trades_;

  void __GroupTradesBySupplier() {
    typename std::vector< Trade<T> >::const_iterator it;
    for (it = trades_.begin(); it != trades_.end(); ++it) {
      trades_by_supplier_[it->bid->bidder()].push_back(*it);
    }
  }
};

} // namespace cyclus

#endif // ifndef CYCLUS_EXECUTOR_TRADE_H_
