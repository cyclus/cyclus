#ifndef CYCLUS_SRC_EXCHANGE_SOLVER_H_
#define CYCLUS_SRC_EXCHANGE_SOLVER_H_

#include <cstddef>

namespace cyclus {

class ExchangeGraph;

/// @class ExchangeSolver
///
/// @brief a very simple interface for solving translated resource exchanges
class ExchangeSolver {
 public:
  explicit ExchangeSolver(bool exclusive_orders = false)
    : exclusive_orders_(exclusive_orders),
      verbose_(false) {}
  virtual ~ExchangeSolver() {}

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
  /// @param cost_add the amount to add to the calculated ratio
  /// @{
  double PseudoCost() { return PseudoCost(1); }
  double PseudoCost(double cost_add);
  /// @}

 protected:
  /// @brief Worker function for solving a graph. This must be implemented by
  /// any solver.
  virtual double SolveGraph() = 0;
  ExchangeGraph* graph_;
  bool exclusive_orders_;
  bool verbose_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_EXCHANGE_SOLVER_H_
