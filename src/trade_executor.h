#ifndef CYCLUS_SRC_TRADE_EXECUTOR_H_
#define CYCLUS_SRC_TRADE_EXECUTOR_H_

#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "context.h"
#include "exchange_context.h"
#include "trade.h"
#include "trader.h"
#include "trader_management.h"

namespace cyclus {

/// @class TradeExecutor::Context
///
/// @brief a holding class for information related to a TradeExecutor
template <class T> struct TradeExecutionContext {
  std::set<Trader*> suppliers;
  std::set<Trader*> requesters;

  // the key is the supplier
  std::map<Trader*, std::vector<Trade<T>>> trades_by_supplier;

  // the key is the requester, values are a vector of the target Trade with the
  // associated response resource provided by the supplier
  std::map<Trader*, std::vector<std::pair<Trade<T>, typename T::Ptr>>>
      trades_by_requester;

  // by convention, the first trader is the supplier, the second is the
  // requester
  std::map<std::pair<Trader*, Trader*>,
           std::vector<std::pair<Trade<T>, typename T::Ptr>>>
      all_trades;
};

/// @class TradeExecutor
///
/// @brief The TradeExecutor is an object whose task is to execute a collection
/// of Trades. Trade Execution takes nominally three steps:
///     #. Grouping all trades by supplier (sender)
///     #. Collecting responses for the group of trades from each supplier
///     #. Grouping all responses by requester (receiver)
///     #. Sending all grouped responses to their respective requester
template <class T> class TradeExecutor {
 public:
  explicit TradeExecutor(const std::vector<Trade<T>>& trades)
      : trades_(trades) {}

  /// @brief execute all trades, collecting responders from bidders and sending
  /// responses to requesters
  ///
  /// @deprecated Use ExecuteTrades(Context*) instead.  This function just
  /// calls ExecuteTrades(NULL).
  void ExecuteTrades() {
    Warn<IO_WARNING>("this function does not record trades to the database");
    ExecuteTrades(NULL);
  }

  /// @brief execute all trades, collecting responders from bidders and sending
  /// responses to requesters
  void ExecuteTrades(Context* ctx) { ExecuteTrades(ctx, NULL); }

  /// @brief execute all trades with access to exchange context for adjusted
  /// preferences
  void ExecuteTrades(Context* ctx, ExchangeContext<T>* ex_ctx) {
    GroupTradesBySupplier(trade_ctx_, trades_);
    GetTradeResponses(trade_ctx_);
    if (ctx) {
      RecordTrades(ctx, ex_ctx);
    }
    SendTradeResources(trade_ctx_);
  }

  /// @brief Record all trades with the appropriate backends
  ///
  /// @param ctx the Context through which communication with backends will
  /// occur
  void RecordTrades(Context* ctx) { RecordTrades(ctx, NULL); }

  /// @brief Record all trades with the appropriate backends, using adjusted
  /// preferences
  ///
  /// @param ctx the Context through which communication with backends will
  /// occur
  /// @param ex_ctx the ExchangeContext containing the adjusted preferences used
  /// by the solver
  void RecordTrades(Context* ctx, ExchangeContext<T>* ex_ctx) {
    // record all trades
    typename std::map<
        std::pair<Trader*, Trader*>,
        std::vector<std::pair<Trade<T>, typename T::Ptr>>>::iterator m_it;
    for (m_it = trade_ctx_.all_trades.begin();
         m_it != trade_ctx_.all_trades.end();
         ++m_it) {
      Agent* supplier = m_it->first.first->manager();
      Agent* requester = m_it->first.second->manager();
      typename std::vector<std::pair<Trade<T>, typename T::Ptr>>& trades =
          m_it->second;
      typename std::vector<std::pair<Trade<T>, typename T::Ptr>>::iterator v_it;
      for (v_it = trades.begin(); v_it != trades.end(); ++v_it) {
        Trade<T>& trade = v_it->first;
        typename T::Ptr rsrc = v_it->second;
        if (rsrc->quantity() > cyclus::eps_rsrc()) {
          // Get original MC and MU
          double original_mc = trade.bid->preference();
          if (std::isnan(original_mc)) {
            original_mc = 0.0;  // NaN means no cost
          }
          double original_mu = trade.request->preference();

          // Get adjusted MC and MU from exchange context
          double adjusted_mc = original_mc;
          double adjusted_mu = original_mu;
          
          if (ex_ctx) {
            auto trader_it = ex_ctx->trader_mc.find(trade.request->requester());
            if (trader_it != ex_ctx->trader_mc.end()) {
              auto request_it = trader_it->second.find(trade.request);
              if (request_it != trader_it->second.end()) {
                auto bid_it = request_it->second.find(trade.bid);
                if (bid_it != request_it->second.end()) {
                  adjusted_mc = bid_it->second;
                }
              }
            }
            
            trader_it = ex_ctx->trader_mu.find(trade.request->requester());
            if (trader_it != ex_ctx->trader_mu.end()) {
              auto request_it = trader_it->second.find(trade.request);
              if (request_it != trader_it->second.end()) {
                auto bid_it = request_it->second.find(trade.bid);
                if (bid_it != request_it->second.end()) {
                  adjusted_mu = bid_it->second;
                }
              }
            }
          }
          
          // Compute arc weight: MC - MU + shift
          // We need to compute shift = max(MU) across all arcs
          // For now, we'll compute it from the graph if available
          // Otherwise, we'll use a reasonable default
          double shift = 0.0;  // Will be computed from graph if available
          double arc_weight = adjusted_mc - adjusted_mu + shift;

          ctx->NewDatum("Transactions")
              ->AddVal("TransactionId", ctx->NextTransactionID())
              ->AddVal("SenderId", supplier->id())
              ->AddVal("ReceiverId", requester->id())
              ->AddVal("ResourceId", rsrc->state_id())
              ->AddVal("Commodity", trade.request->commodity())
              ->AddVal("Time", ctx->time())
              ->AddVal("MC", adjusted_mc)
              ->AddVal("MU", adjusted_mu)
              ->AddVal("ArcWeight", arc_weight)
              ->Record();
        }
      }
    }
  }

  inline const TradeExecutionContext<T>& trade_ctx() const {
    return trade_ctx_;
  }
  inline TradeExecutionContext<T>& trade_ctx() { return trade_ctx_; }

 private:
  const std::vector<Trade<T>>& trades_;
  TradeExecutionContext<T> trade_ctx_;
};

/// @brief populates suppliers_, requesters_, and trades_by_supplier_
template <class T>
void GroupTradesBySupplier(TradeExecutionContext<T>& trade_ctx,
                           const std::vector<Trade<T>>& trades) {
  typename std::vector<Trade<T>>::const_iterator it;
  for (it = trades.begin(); it != trades.end(); ++it) {
    trade_ctx.trades_by_supplier[it->bid->bidder()].push_back(*it);
    trade_ctx.suppliers.insert(it->bid->bidder());
    trade_ctx.requesters.insert(it->request->requester());
  }
}

/// @brief queries each supplier for the responses to thier matched trade and
/// populates trades_by_requester_ and all_trades_ with the results
template <class T>
static void GetTradeResponses(TradeExecutionContext<T>& trade_ctx) {
  std::set<Trader*>::iterator it;
  for (it = trade_ctx.suppliers.begin(); it != trade_ctx.suppliers.end();
       ++it) {
    // get responses
    Trader* supplier = *it;
    std::vector<std::pair<Trade<T>, typename T::Ptr>> responses;
    PopulateTradeResponses(supplier, trade_ctx.trades_by_supplier[supplier],
                           responses);

    // populate containers
    typename std::vector<std::pair<Trade<T>, typename T::Ptr>>::iterator r_it;
    for (r_it = responses.begin(); r_it != responses.end(); ++r_it) {
      // @todo unsure if this is needed...
      // Trade<T>& trade = r_it->first;
      // typename T::Ptr rsrc= r_it->second;
      // if (rsrc->quantity() != trade.amt) {
      //   throw ValueError("Trade amt and resource qty must match");
      // }
      Trader* requester = r_it->first.request->requester();
      trade_ctx.trades_by_requester[requester].push_back(*r_it);
      trade_ctx.all_trades[std::make_pair(supplier, requester)].push_back(
          *r_it);
    }
  }
}

template <class T>
static void SendTradeResources(TradeExecutionContext<T>& trade_ctx) {
  std::set<Trader*>::iterator it;
  for (it = trade_ctx.requesters.begin(); it != trade_ctx.requesters.end();
       ++it) {
    Trader* requester = *it;
    AcceptTrades(requester, trade_ctx.trades_by_requester[requester]);
  }
}

}  // namespace cyclus

#endif  // CYCLUS_SRC_TRADE_EXECUTOR_H_
