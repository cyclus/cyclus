#ifndef CYCLUS_SRC_EXCHANGE_SOLVER_H_
#define CYCLUS_SRC_EXCHANGE_SOLVER_H_

#include <cstddef>

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
  
  /// return the cost of an arc
  static double Cost(const Arc& a, bool exclusive_orders = kDefaultExclusive);

  explicit ExchangeSolver(bool exclusive_orders = kDefaultExclusive)
    : exclusive_orders_(exclusive_orders),
      sim_ctx_(NULL),
      verbose_(false) {}
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
    if (graph != NULL)
      graph_ = graph;
    return this->SolveGraph();
  }

  /// @brief Calculates the ratio of the maximum objective coefficient to
  /// minimum unit capacity plus an added cost. This is guaranteed to be larger
  /// than any other arc cost measure and can be used as a cost for unmet
  /// demand.
  /// @param cost_factor the additional cost for false arc costs, i.e., max_cost
  /// * (1 + cost_factor)
  /// @{
  double PseudoCost();
  double PseudoCost(double cost_factor);
  double PseudoCostByCap(double cost_factor);
  double PseudoCostByPref(double cost_factor);
  /// @}
  
  /// return the cost of an arc
  inline double ArcCost(const Arc& a) { return Cost(a, exclusive_orders_); }
  
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
