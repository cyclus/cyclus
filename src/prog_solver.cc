#include "prog_solver.h"

#include "OsiSolverInterface.hpp"

#include "prog_translator.h"
#include "greedy_solver.h"
#include "solver_factory.h"

namespace cyclus {

void Report(OsiSolverInterface* iface) {
  std::cout << iface->getNumCols() << " total variables, "
            << iface->getNumIntegers() << " integer.\n";
  std::cout << iface->getNumRows() << " constraints\n";
}

ProgSolver::ProgSolver(std::string solver_t)
    : solver_t_(solver_t),
      tmax_(ProgSolver::KOptimizeDefaultTimeout),
      verbose_(false),
      ExchangeSolver(false) {}

ProgSolver::ProgSolver(std::string solver_t, bool exclusive_orders)
    : solver_t_(solver_t),
      tmax_(ProgSolver::KOptimizeDefaultTimeout),
      verbose_(false),
      ExchangeSolver(exclusive_orders) {}

ProgSolver::ProgSolver(std::string solver_t, double tmax)
    : solver_t_(solver_t),
      tmax_(tmax),
      verbose_(false),
      ExchangeSolver(false) {}

ProgSolver::ProgSolver(std::string solver_t, double tmax, bool exclusive_orders,
                       bool verbose)
    : solver_t_(solver_t),
      tmax_(tmax),
      verbose_(verbose),
      ExchangeSolver(exclusive_orders) {}

ProgSolver::~ProgSolver() {}

double ProgSolver::SolveGraph() {
  SolverFactory sf(solver_t_, tmax_);
  OsiSolverInterface* iface = sf.get();
  try {
    // get greedy solution
    GreedySolver greedy(exclusive_orders_);
    double greedy_obj = greedy.Solve(graph_);
    graph_->ClearMatches();
    
    // translate graph to iface instance
    double pseudo_cost = PseudoCost(); // from ExchangeSolver API
    ProgTranslator xlator(graph_, iface, exclusive_orders_, pseudo_cost);
    xlator.ToProg();

    // set noise level
    CoinMessageHandler h;
    h.setLogLevel(0);
    if (verbose_) {
      Report(iface);
      h.setLogLevel(4);
    }
    iface->passInMessageHandler(&h);
    if (verbose_) {
      std::cout << "Solving problem, message handler has log level of "
                << iface->messageHandler()->logLevel() << "\n";
    }

    // solve and back translate
    SolveProg(iface, greedy_obj, verbose_);
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
