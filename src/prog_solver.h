#ifndef CYCLUS_PROG_SOLVER_H_
#define CYCLUS_PROG_SOLVER_H_

#include "exchange_graph.h"
#include "exchange_solver.h"

namespace cyclus {

class ExchangeGraph;
  
/// @brief The ProgSolver provides the implementation for a mathematical
/// programming solution to a resource exchange graph.
class ProgSolver: public ExchangeSolver {
 public:
  ProgSolver(bool exclusive_orders = false);
  ProgSolver(ExchangeGraph* g, bool exclusive_orders = false);
  
  virtual ~ProgSolver();

  /// @brief the ProgSolver solves an ExchangeGraph...
  virtual void Solve();

 private:
  bool exclusive_orders_;
};

} // namespace cyclus

#endif // ifndef CYCLUS_PROG_SOLVER_H_
