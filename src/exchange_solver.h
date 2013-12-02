#ifndef CYCLUS_EXCHANGE_SOLVER_H_
#define CYCLUS_EXCHANGE_SOLVER_H_

namespace cyclus {

class ExchangeGraph;
  
/// @class ExchangeSolver
///
/// @brief a very simple interface for solving translated resource exchanges
class ExchangeSolver {
 public:
  ExchangeSolver() {};
  explicit ExchangeSolver(ExchangeGraph* g) : graph_(g) {};
  virtual ~ExchangeSolver() {};

  inline void set_graph(ExchangeGraph* graph) { graph_ = graph; }
  inline const ExchangeGraph* set_graph() const { return graph_; }

  /// @brief any solver must implement a Solve() function
  virtual void Solve() = 0;

 protected:
  ExchangeGraph* graph_;
};

} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_SOLVER_H_
