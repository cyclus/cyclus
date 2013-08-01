#ifndef CYCLOPTS_SOLVER_H_
#define CYCLOPTS_SOLVER_H_

#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

namespace cyclopts {
class Solver;
typedef boost::shared_ptr<Solver> SolverPtr;
}

#include "function.h"
#include "variable.h"

namespace cyclopts {
/// abstract base class for different types of constraint program solvers
class Solver {
 public:
  /// constructor
  Solver();

  /// virtual destructor
  virtual ~Solver() {};

  /// solve method to be overloaded by derived classes
  virtual void Solve(std::vector<VariablePtr>& variables, ObjFuncPtr obj, 
                     std::vector<ConstraintPtr>& constraints) = 0;

 protected:
  /// the indices used for each variable
  std::map<VariablePtr,int> index_;

  /// match variable pointers to indices, populating indicies_
  void PopulateIndices(std::vector<VariablePtr>& variables);
};
}

#endif
