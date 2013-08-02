#ifndef CYCLOPTS_SOLVER_H_
#define CYCLOPTS_SOLVER_H_

#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

namespace cyclus {
namespace cyclopts {
class Solver;
typedef boost::shared_ptr<Solver> SolverPtr;
} // namespace cyclopts
} // namespace cyclus

#include "function.h"
#include "variable.h"

namespace cyclus {
namespace cyclopts {
/// abstract base class for different types of constraint program solvers
class Solver {
 public:
  /// constructor
  Solver();

  /// virtual destructor
  virtual ~Solver() {};

  /// solve method to be overloaded by derived classes
  virtual void Solve(std::vector<cyclus::cyclopts::VariablePtr>& variables, 
                     cyclus::cyclopts::ObjFuncPtr obj, 
                     std::vector<cyclus::cyclopts::ConstraintPtr>& constraints) = 0;

 protected:
  /// the indices used for each variable
  std::map<cyclus::cyclopts::VariablePtr,int> index_;

  /// match variable pointers to indices, populating indicies_
  void PopulateIndices(std::vector<cyclus::cyclopts::VariablePtr>& variables);
};
} // namespace cyclopts
} // namespace cyclus

#endif
