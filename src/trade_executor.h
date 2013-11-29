#ifndef CYCLUS_TRADE_EXECUTOR_H_
#define CYCLUS_TRADE_EXECUTOR_H_

#include <map>
#include <set>
#include <utility>
#include <vector>

#include "context.h"
#include "trade.h"
#include "trader.h"
#include "trader_management.h"

namespace cyclus {

/// @class TradeExecutor
///
/// @brief The TradeExecutor is an object whose task is to execute a collection
/// of Trades. Trade Execution takes nominally three steps:
///     #. Grouping all trades by supplier (sender)
///     #. Collecting responses for the group of trades from each supplier
///     #. Grouping all responses by requester (receiver)
///     #. Sending all grouped responses to their respective requester
template <class T>
class TradeExecutor {
 public:
  explicit TradeExecutor(const std::vector< Trade<T> >& trades)
    : trades_(trades) {};

  /// @brief execute all trades, collecting responsers from bidders and sending
  /// responses to requesters
  void ExecuteTrades() {
    __GroupTradesBySupplier();
    __GetTradeResponses();
    __SendTradeResources();
  };

  /// @brief Record all trades with the appropriate backends
  ///
  /// @param ctx the Context through which communication with backends will
  /// occur
  void RecordTrades(Context* ctx) {
    // record all trades
    typename std::map<std::pair<Trader*, Trader*>,
        std::vector< std::pair<Trade<T>, typename T::Ptr> > >::iterator m_it;
    for (m_it = all_trades_.begin(); m_it != all_trades_.end(); ++m_it) {
      Trader* supplier = m_it->first.first;
      Trader* requester = m_it->first.second;
      typename std::vector< std::pair<Trade<T>, typename T::Ptr> >& trades =
          m_it->second;
      typename std::vector< std::pair<Trade<T>,
          typename T::Ptr> >::iterator v_it;
      for (v_it = trades.begin(); v_it != trades.end(); ++v_it) {
        Trade<T>& trade = v_it->first;
        typename T::Ptr rsrc =  v_it->second;
        ctx->NewEvent("Transactions")
            ->AddVal("ID", ctx->NextTransactionID())
            ->AddVal("SenderID", supplier->id())
            ->AddVal("ReceiverID", requester->id())
            ->AddVal("ResourceID", rsrc->id())
            ->AddVal("Commodity", trade.request->commodity())
            ->AddVal("Price", trade.price)
            ->AddVal("Time", ctx->time())
            ->Record();
      }
    }
  }
  
  /* -------------------- private methods and members -------------------------- */
  const std::vector< Trade<T> >& trades_;

  std::set<Trader*> suppliers_;
  
  std::set<Trader*> requesters_;

  // the key is the supplier
  std::map<Trader*,
      std::vector< Trade<T> > > trades_by_supplier_;
  
  // the key is the requester, values are a vector of the target Trade with the
  // associated response resource provided by the supplier
  std::map<Trader*,
      std::vector< std::pair<Trade<T>, typename T::Ptr> > >
      trades_by_requester_;

  // by convention, the first trader is the supplier, the second is the
  // requester
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
        // @todo unsure if this is needed...
        // Trade<T>& trade = r_it->first;
        // typename T::Ptr rsrc= r_it->second;
        // if (rsrc->quantity() != trade.amt) {
        //   throw ValueError("Trade amt and resource qty must match");
        // }
        Trader* requester = r_it->first.request->requester();
        trades_by_requester_[requester].push_back(*r_it);
        all_trades_[std::make_pair(supplier, requester)].push_back(*r_it);
      }
    }
  }
  
  void __SendTradeResources() {
    std::set<Trader*>::iterator it;
    for (it = requesters_.begin(); it != requesters_.end(); ++it) {
      // get responses
      Trader* requester = *it;
      AcceptTrades(requester, trades_by_requester_[requester]);
    }
  }
};

} // namespace cyclus

#endif // ifndef CYCLUS_EXECUTOR_TRADE_H_
