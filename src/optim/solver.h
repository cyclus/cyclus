#ifndef CYCLUS_CYCLOPTS_SOLVER_H_
#define CYCLUS_CYCLOPTS_SOLVER_H_

#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

namespace cyclus {
namespace optim {
class Solver;
typedef boost::shared_ptr<Solver> SolverPtr;
} // namespace optim
} // namespace cyclus

#include "function.h"
#include "variable.h"

namespace cyclus {
namespace optim {
/// abstract base class for different types of constraint program solvers
class Solver {
 public:
  /// constructor
  Solver();

  /// virtual destructor
  virtual ~Solver() {};

  /// solve method to be overloaded by derived classes
  virtual void Solve(
      std::vector<cyclus::optim::VariablePtr>& variables, 
      cyclus::optim::ObjFuncPtr obj, 
      std::vector<cyclus::optim::ConstraintPtr>& constraints) = 0;

 protected:
  /// the indices used for each variable
  std::map<cyclus::optim::VariablePtr, int> index_;

  /// match variable pointers to indices, populating indicies_
  void PopulateIndices(std::vector<cyclus::optim::VariablePtr>& variables);
};
} // namespace optim
} // namespace cyclus

#endif
