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

  inline void graph(ExchangeGraph* graph) { graph_ = graph; }
  inline ExchangeGraph* graph() { return graph_; }
  
  /// @brief interface for solving a given exchange graph
  /// @param a pointer to the graph to be solved
  void Solve(ExchangeGraph* graph = NULL) {
    if (graph != NULL)
      graph_ = graph;
    this->SolveGraph();
  }
  
 protected:
  /// @brief Worker function for solving a graph. This must be implemented by
  /// any solver.
  virtual void SolveGraph() = 0;
  ExchangeGraph* graph_;
  bool exclusive_orders_;
};

} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_SOLVER_H_
