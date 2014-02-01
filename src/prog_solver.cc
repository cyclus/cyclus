#include "prog_solver.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ProgSolver::ProgSolver(bool exclusive_orders)
  : exclusive_orders_(exclusive_orders) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ProgSolver::ProgSolver(ExchangeGraph* g, bool exclusive_orders)
  : exclusive_orders_(exclusive_orders),
    ExchangeSolver(g) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ProgSolver::~ProgSolver() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ProgSolver::Solve() {
  // translate exchange graph to lp/milp instance
  // call solver
  // backtranslate to matched arcs
}

} // namespace cyclus
