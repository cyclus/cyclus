#include "prog_solver.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ProgSolver::ProgSolver(bool exclusive_orders)
  : ExchangeSolver(exclusive_orders) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ProgSolver::~ProgSolver() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ProgSolver::Solve() {
  // translate exchange graph to lp/milp instance
  // call solver
  // backtranslate to matched arcs
}

} // namespace cyclus
