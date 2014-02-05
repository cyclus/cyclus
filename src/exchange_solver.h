#ifndef CYCLUS_EXCHANGE_SOLVER_H_
#define CYCLUS_EXCHANGE_SOLVER_H_

namespace cyclus {

class ExchangeGraph;
  
/// @class ExchangeSolver
///
/// @brief a very simple interface for solving translated resource exchanges
class ExchangeSolver {
 public:
  explicit ExchangeSolver(bool exclusive_orders = false) {};
  virtual ~ExchangeSolver() {};

  /// @brief interface for solving a given exchange graph
  /// @param a pointer to the graph to be solved
  void Solve(ExchangeGraph* graph) {
    graph_ = graph;
    this->Solve();
  }
  
 protected:
  /// @brief Worker function for solving a graph. This must be implemented by
  /// any solver.
  virtual void Solve() = 0;
  ExchangeGraph* graph_;
  bool exclusive_orders_;
};

} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_SOLVER_H_
