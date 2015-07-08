#include "prog_solver.h"

#include <sstream>

#include "context.h"
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
      tmax_(ProgSolver::kDefaultTimeoutOptimize),
      verbose_(false),
      mps_(false),
      ExchangeSolver(false) {}

ProgSolver::ProgSolver(std::string solver_t, bool exclusive_orders)
    : solver_t_(solver_t),
      tmax_(ProgSolver::kDefaultTimeoutOptimize),
      verbose_(false),
      mps_(false),
      ExchangeSolver(exclusive_orders) {}

ProgSolver::ProgSolver(std::string solver_t, double tmax)
    : solver_t_(solver_t),
      tmax_(tmax),
      verbose_(false),
      mps_(false),
      ExchangeSolver(false) {}

ProgSolver::ProgSolver(std::string solver_t, double tmax, bool exclusive_orders,
                       bool verbose, bool mps)
    : solver_t_(solver_t),
      tmax_(tmax),
      verbose_(verbose),
      mps_(mps),
      ExchangeSolver(exclusive_orders) {}

ProgSolver::~ProgSolver() {}

void ProgSolver::WriteMPS() {
  std::stringstream ss;
  ss << "exchng_" << sim_ctx_->time();
  iface_->writeMps(ss.str().c_str());
}

double ProgSolver::SolveGraph() {
  SolverFactory sf(solver_t_, tmax_);
  iface_ = sf.get();
  try {
    // get greedy solution
    GreedySolver greedy(exclusive_orders_);
    double greedy_obj = greedy.Solve(graph_);
    graph_->ClearMatches();
    
    // translate graph to iface_ instance
    double pseudo_cost = PseudoCost(); // from ExchangeSolver API
    ProgTranslator xlator(graph_, iface_, exclusive_orders_, pseudo_cost);
    xlator.ToProg();
    if (mps_)
      WriteMPS();
    
    // set noise level
    CoinMessageHandler h;
    h.setLogLevel(0);
    if (verbose_) {
      Report(iface_);
      h.setLogLevel(4);
    }
    iface_->passInMessageHandler(&h);
    if (verbose_) {
      std::cout << "Solving problem, message handler has log level of "
                << iface_->messageHandler()->logLevel() << "\n";
    }
        
    // solve and back translate
    SolveProg(iface_, greedy_obj, verbose_);

    xlator.FromProg();
  } catch(...) {
    delete iface_;
    throw;
  }
  double ret = iface_->getObjValue(); 
  delete iface_;
  return ret;
}

}  // namespace cyclus
