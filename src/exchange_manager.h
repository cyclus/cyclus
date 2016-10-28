#ifndef CYCLUS_SRC_EXCHANGE_MANAGER_H_
#define CYCLUS_SRC_EXCHANGE_MANAGER_H_

#include <algorithm>

#include "exchange_graph.h"
#include "exchange_solver.h"
#include "exchange_translator.h"
#include "resource_exchange.h"
#include "trade_executor.h"
#include "trader_management.h"
#include "env.h"

namespace cyclus {

/// @class ExchangeManager
///
/// @brief The ExchangeManager is designed to house all of the internals
/// involved in executing a resource exchange. At a given timestep, assuming a
/// simulation context, ctx, and resource type, ResourceType, it can be invoked
/// by:
///
/// @code
/// ExchangeManager<ResourceType> manager(ctx);
/// manager.Execute();
/// @endcode
template <class T>
class ExchangeManager {
 public:
  ExchangeManager(Context* ctx) : ctx_(ctx), debug_(false) {
    debug_ = Env::GetEnv("CYCLUS_DEBUG_DRE").size() > 0;
  }

  /// @brief execute the full resource sequence
  void Execute() {
    // collect resource exchange information
    ResourceExchange<T> exchng(ctx_);
    exchng.AddAllRequests();
    exchng.AddAllBids();
    exchng.AdjustAll();
    CLOG(LEV_DEBUG1) << "done with info gathering";
    
    if (debug_)
      RecordDebugInfo(exchng.ex_ctx());

    if (exchng.Empty())
      return; // empty exchange, move on

    // translate graph
    ExchangeTranslator<T> xlator(&exchng.ex_ctx());
    CLOG(LEV_DEBUG1) << "translating graph...";
    ExchangeGraph::Ptr graph = xlator.Translate();
    CLOG(LEV_DEBUG1) << "graph translated!";

    // solve graph
    CLOG(LEV_DEBUG1) << "solving graph...";
    ctx_->solver()->Solve(graph.get());
    CLOG(LEV_DEBUG1) << "graph solved!";

    // get trades
    std::vector< Trade<T> > trades;
    xlator.BackTranslateSolution(graph->matches(), trades);
    CLOG(LEV_DEBUG1) << "trades translated!";

    // execute trades!
    TradeExecutor<T> exec(trades);
    exec.ExecuteTrades(ctx_);
  }

 private:
  void RecordDebugInfo(ExchangeContext<T>& exctx) {
    typename std::vector<typename RequestPortfolio<T>::Ptr>::iterator it;
    for (it = exctx.requests.begin(); it != exctx.requests.end(); ++it) {
      std::vector<Request<T>*> reqs = (*it)->requests();
      typename std::vector<Request<T>*>::iterator it2;
      for (it2 = reqs.begin(); it2 != reqs.end(); ++it2) {
        Request<T>* r = *it2;
        std::stringstream ss;
        ss << ctx_->time() << "_" << r;
        ctx_->NewDatum("DebugRequests")
          ->AddVal("Time", ctx_->time())
          ->AddVal("ReqId", ss.str())
          ->AddVal("RequesterID", r->requester()->manager()->id())
          ->AddVal("Commodity", r->commodity())
          ->AddVal("Preference", r->preference())
          ->AddVal("Exclusive", r->exclusive())
          ->AddVal("ResType", r->target()->type())
          ->AddVal("Quantity", r->target()->quantity())
          ->AddVal("ResUnits", r->target()->units())
          ->Record();
      }
    }

    typename std::vector<typename BidPortfolio<T>::Ptr>::iterator it3;
    for (it3 = exctx.bids.begin(); it3 != exctx.bids.end(); ++it3) {
      std::set<Bid<T>*> bids = (*it3)->bids();
      typename std::set<Bid<T>*>::iterator it4;
      for (it4 = bids.begin(); it4 != bids.end(); ++it4) {
        Bid<T>* b = *it4;
        Request<T>* r = b->request();
        double pref = exctx.trader_prefs[r->requester()][r][b];
        std::stringstream ss;
        ss << ctx_->time() << "_" << b->request();
        ctx_->NewDatum("DebugBids")
          ->AddVal("ReqId", ss.str())
          ->AddVal("BidderId", b->bidder()->manager()->id())
          ->AddVal("BidQuantity", b->offer()->quantity())
          ->AddVal("Exclusive", b->exclusive())
          ->AddVal("Preference", pref)
          ->Record();
      }
    }
  }

  bool debug_;
  Context* ctx_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_EXCHANGE_MANAGER_H_
