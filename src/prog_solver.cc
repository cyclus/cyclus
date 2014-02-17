#include "prog_solver.h"

#include "OsiSolverInterface.hpp"

#include "prog_translator.h"
#include "solver_factory.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ProgSolver::ProgSolver(std::string solver_t, bool exclusive_orders)
  : solver_t_(solver_t), ExchangeSolver(exclusive_orders) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ProgSolver::~ProgSolver() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ProgSolver::Solve() {
  SolverFactory sf(solver_t_);
  OsiSolverInterface* iface = sf.get();
  try {
    ProgTranslator xlator(graph_, iface, exclusive_orders_);
    xlator.ToProg();
    SolveProg(iface);
    xlator.FromProg();
  } catch(...) {
    delete iface;
    throw;
  }
  delete iface;
}

} // namespace cyclus
