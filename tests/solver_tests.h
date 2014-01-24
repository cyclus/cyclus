#ifndef CYCLUS_TESTS_SOLVER_TESTS_H_
#define CYCLUS_TESTS_SOLVER_TESTS_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "exchange_graph.h"
#include "exchange_solver.h"
#include "greedy_solver.h"

namespace cyclus {

/// A factory class to supply exchange solver tests with configured
/// ExchangeSolvers.
/// To add a new solver, add a unique name for it to solver_types_ and its
/// configureation to the Solver factory member function
class SolverFactory {
 public:
  SolverFactory() {
    solver_types_.push_back("greedy");
  }

  /// A factory for any known configured ExchangeSolver
  boost::shared_ptr<ExchangeSolver> Solver(std::string solver_type,
                                           ExchangeGraph* g) {
    boost::shared_ptr<ExchangeSolver> ret;
    if (solver_type == "greedy") {
      ret = boost::shared_ptr<ExchangeSolver>(new GreedySolver(g));
    }
    return ret;
  }
  
  std::vector<std::string> solver_types() { return solver_types_; }

 private:
  std::vector<std::string> solver_types_;
};

} // namespace cyclus

#endif // ifndef CYCLUS_TESTS_SOLVER_TESTS_H_
