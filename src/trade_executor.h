#ifndef CYCLUS_TRADE_EXECUTOR_H_
#define CYCLUS_TRADE_EXECUTOR_H_

#include <map>
#include <set>
#include <utility>
#include <vector>

#include "trade.h"
#include "trader.h"
#include "trader_management.h"

namespace cyclus {

template <class T>
class TradeExecutor {
 public:
  explicit TradeExecutor(const std::vector< Trade<T> >& trades)
    : trades_(trades) { };

  void ExecuteTrades() {
    __GroupTradesBySupplier();
    __GetTradeResponses();
    // send trades by requester
  };
  
  void RecordTrades() {
    // record all trades
  };
  
  /* -------------------- private methods and members -------------------------- */
  const std::vector< Trade<T> >& trades_;

  // all suppliers
  std::set<Trader*> suppliers_;
  
  // all requesters
  std::set<Trader*> requesters_;
  
  // map of supplier to container of all trades associated with that supplier
  std::map<Trader*,
      std::vector< Trade<T> > > trades_by_supplier_;
  
  std::map<Trader*,
      std::vector< std::pair<Trade<T>, typename T::Ptr> > >
      trades_by_requester_;
  
  std::map<std::pair<Trader*, Trader*>,
      std::vector< std::pair<Trade<T>, typename T::Ptr> > > all_trades_;

  /// @brief populates suppliers_, requesters_, and trades_by_supplier_
  void __GroupTradesBySupplier() {
    typename std::vector< Trade<T> >::const_iterator it;
    for (it = trades_.begin(); it != trades_.end(); ++it) {
      trades_by_supplier_[it->bid->bidder()].push_back(*it);
      suppliers_.insert(it->bid->bidder());
      requesters_.insert(it->request->requester());
    }
  }

  /// @brief queries each supplier for the responses to thier matched trade and
  /// populates trades_by_requester_ and all_trades_ with the results
  void __GetTradeResponses() {
    std::set<Trader*>::iterator it;
    for (it = suppliers_.begin(); it != suppliers_.end(); ++it) {
      // get responses
      Trader* supplier = *it;
      std::vector< std::pair<Trade<T>, typename T::Ptr> > responses;
      PopulateTradeResponses(supplier, trades_by_supplier_[supplier],
                             responses);

      // populate containers
      typename std::vector< std::pair<Trade<T>, typename T::Ptr> >::iterator r_it;
      for (r_it = responses.begin(); r_it != responses.end(); ++r_it) {
        Trader* requester = r_it->first.request->requester();
        trades_by_requester_[requester].push_back(*r_it);
        all_trades_[std::make_pair(supplier, requester)].push_back(*r_it);
      }
    }
  }
};

} // namespace cyclus

#endif // ifndef CYCLUS_EXECUTOR_TRADE_H_
