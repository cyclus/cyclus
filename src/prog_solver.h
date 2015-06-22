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
  static const int KOptimizeDefaultTimeout = 5 * 60; // 5 * 60 s/min == 5 minutes

  /// @param solver_t the solver type, either "cbc" or "clp"
  /// @param tmax the maximum solution time, default kOptimizeDefaultTimeout
  /// @param exclusive_orders whether all orders must be exclusive or not,
  /// default false
  /// @{
  ProgSolver(std::string solver_t);
  ProgSolver(std::string solver_t, double tmax);
  ProgSolver(std::string solver_t, bool exclusive_orders);
  ProgSolver(std::string solver_t, double tmax, bool exclusive_orders);
  /// @}
  virtual ~ProgSolver();

 protected:
  /// @brief the ProgSolver solves an ExchangeGraph...
  virtual double SolveGraph();
  
 private:
  std::string solver_t_;
  double tmax_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_PROG_SOLVER_H_
