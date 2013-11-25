#ifndef CYCLUS_EXCHANGE_MANAGER_H
#define CYCLUS_EXCHANGE_MANAGER_H

#include <algorithm>

#include "exchange_graph.h"
#include "exchange_solver.h"
#include "exchange_translator.h"
#include "resource_exchange.h"
#include "trader_management.h"

namespace cyclus {

/// @class ExchangeManager
///
/// @brief The ExchangeManager is designed to house all of the internals
/// involved in executing a resource exchange. At a given timestep, assuming a
/// simulation context, ctx, a resource solver, solver, and resource type,
/// ResourceType, it can be invoked by:
///
/// @code
/// ExchangeManager<ResourceType> manager(ctx, solver);
/// manager.Execute();
/// @endcode
template <class T>
class ExchangeManager {
 public:
  ExchangeManager(Context* ctx, ExchangeSolver* solver)
    : ctx_(ctx),
      solver_(solver) { };

  /// @brief execute the full resource sequence
  void Execute() {
    // collect resource exchange information
    ResourceExchange<T> exchng(ctx_);
    exchng.AddAllRequests();
    exchng.AddAllBids();
    exchng.DoAllAdjustments();
    CLOG(LEV_DEBUG1) << "done with info gathering";
    
    // translate graph
    ExchangeTranslator<T> xlator(&exchng.ex_ctx());
    CLOG(LEV_DEBUG1) << "translating graph...";
    ExchangeGraph::Ptr graph = xlator.Translate();
    CLOG(LEV_DEBUG1) << "graph translated!";
    
    // solve graph
    solver_->set_graph(graph.get());
    CLOG(LEV_DEBUG1) << "solving graph...";
    solver_->Solve();
    CLOG(LEV_DEBUG1) << "graph solved!";

    // get trades
    std::vector< Trade<T> > trades;
    xlator.BackTranslateSolution(graph->matches, trades);
    CLOG(LEV_DEBUG1) << "trades translated!";

    std::for_each(trades.begin(), trades.end(), ExecuteTrade<T>);
  }

  /* -------------------- private methods and members ----------------------- */  
  Context* ctx_;
  ExchangeSolver* solver_;
};

} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_MANAGER_H
