#ifndef CYCLUS_SRC_PROG_SOLVER_H_
#define CYCLUS_SRC_PROG_SOLVER_H_

#include <string>

#include "exchange_graph.h"
#include "exchange_solver.h"

namespace cyclus {

class ExchangeGraph;

/// @brief The ProgSolver provides the implementation for a mathematical
/// programming solution to a resource exchange graph.
class ProgSolver: public ExchangeSolver {
 public:
  ProgSolver(std::string solver_t, bool exclusive_orders = false);
  virtual ~ProgSolver();

 protected:
  /// @brief the ProgSolver solves an ExchangeGraph...
  virtual double SolveGraph();
  
 private:
  std::string solver_t_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_PROG_SOLVER_H_
