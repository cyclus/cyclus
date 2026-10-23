#ifndef CYCLUS_SRC_EXCHANGE_SOLVER_H_
#define CYCLUS_SRC_EXCHANGE_SOLVER_H_

#include <cstddef>

#include "error.h"

namespace cyclus {

class Context;
class ExchangeGraph;
class Arc;

/// @class ExchangeSolver
///
/// @brief a very simple interface for solving translated resource exchanges
class ExchangeSolver {
 public:
  /// default value to allow exclusive orders or not
  static const bool kDefaultExclusive = true;

  /// @brief Returns an arc's effective objective coefficient.
  ///
  /// For an exclusive arc when exclusive orders are enabled, the stored arc cost
  /// is scaled by the exclusive quantity. An arc with a non-positive exclusive
  /// quantity is left unscaled.
  ///
  /// @param a The exchange arc to evaluate.
  /// @param exclusive_orders Whether to apply exclusive-order scaling.
  /// @return The effective objective coefficient for the arc.
  static double Cost(const Arc& a, bool exclusive_orders = kDefaultExclusive);

  explicit ExchangeSolver(bool exclusive_orders = kDefaultExclusive)
      : exclusive_orders_(exclusive_orders), sim_ctx_(NULL), verbose_(false) {}
  virtual ~ExchangeSolver() {}

  /// simulation context get/set
  /// @{
  inline void sim_ctx(Context* c) { sim_ctx_ = c; }
  inline Context* sim_ctx() { return sim_ctx_; }
  /// @}

  /// tell the solver to be verbose
  inline void verbose() { verbose_ = true; }
  inline void graph(ExchangeGraph* graph) { graph_ = graph; }
  inline ExchangeGraph* graph() const { return graph_; }

  /// @brief interface for solving a given exchange graph
  /// @param a pointer to the graph to be solved
  double Solve(ExchangeGraph* graph = NULL) {
    if (graph != NULL) graph_ = graph;
    return this->SolveGraph();
  }

  /// @brief Calculates the default faux-arc penalty for unmet demand.
  ///
  /// Uses the default relative margin factor of 0.1.
  ///
  /// @return A penalty greater than every finite effective real-arc cost, or
  ///         0.0 when the graph has no arcs.
  /// @throws ValueError if an effective arc cost is not finite.
  double PseudoCost();

  /// @brief Calculates a faux-arc penalty for unmet demand.
  ///
  /// @param cost_factor The positive relative margin applied to the largest
  ///                    effective arc cost. Should be finite and positive.
  /// @return A penalty greater than every finite effective real-arc cost, or
  ///         0.0 when the graph has no arcs.
  double PseudoCost(double cost_factor);

  /// @brief Calculates a capacity-derived faux-arc penalty for unmet demand.
  ///
  /// The penalty is based on the maximum effective arc coefficient and the
  /// minimum unit capacity found in the exchange graph.
  ///
  /// @param cost_factor The relative margin applied to the calculated penalty.
  /// @return A capacity-derived faux-arc penalty.
  /// @warning Requires valid, non-empty positive unit-capacity data in the
  ///          exchange graph.
  double PseudoCostByCap(double cost_factor);

  /// @brief Calculates a penalty for the faux arc representing unmet demand.
  ///
  /// The returned cost is strictly greater than every finite effective real-arc
  /// cost in the exchange graph, so a solver minimizes unmet demand whenever a
  /// real trade is feasible. The penalty is the largest effective arc cost plus
  /// a scale-aware positive margin.
  ///
  /// @param cost_factor The positive relative margin applied to the largest
  ///                    effective arc cost.
  /// @return The faux-arc penalty, or 0.0 when the graph has no arcs.
  /// @throws ValueError if an effective arc cost is not finite.
  double PseudoCostByArcCost(double cost_factor);
  /// @}

  /// return the cost of an arc (instance method that uses the solver's graph)
  inline double arc_cost(const Arc& a) {
    return Cost(a, exclusive_orders_);
  }

 protected:
  /// @brief Worker function for solving a graph. This must be implemented by
  /// any solver.
  virtual double SolveGraph() = 0;
  ExchangeGraph* graph_;
  bool exclusive_orders_;
  bool verbose_;
  Context* sim_ctx_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_EXCHANGE_SOLVER_H_
