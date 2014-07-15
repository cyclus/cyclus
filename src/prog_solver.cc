#include "prog_solver.h"

#include "OsiSolverInterface.hpp"

#include "prog_translator.h"
#include "solver_factory.h"

namespace cyclus {

void Report(OsiSolverInterface* iface) {
  std::cout << iface->getNumCols() << " total variables, "
            << iface->getNumIntegers() << " integer.\n";
  std::cout << iface->getNumRows() << " constraints\n";
}

ProgSolver::ProgSolver(std::string solver_t, bool exclusive_orders)
    : solver_t_(solver_t),
      ExchangeSolver(exclusive_orders) {}

ProgSolver::~ProgSolver() {}

double ProgSolver::SolveGraph() {
  SolverFactory sf(solver_t_);
  OsiSolverInterface* iface = sf.get();
  try {
    ProgTranslator xlator(graph_, iface, exclusive_orders_);
    xlator.ToProg();
    CoinMessageHandler h;
    h.setLogLevel(0);
    if (verbose_)
      Report(iface);
      h.setLogLevel(4);
    iface->passInMessageHandler(&h);
    SolveProg(iface, verbose_);
    xlator.FromProg();
  } catch(...) {
    delete iface;
    throw;
  }
  double ret = iface->getObjValue(); 
  delete iface;
  return ret;
}

}  // namespace cyclus
