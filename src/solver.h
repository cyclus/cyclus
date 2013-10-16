#ifndef CYCLUS_CYCLOPTS_SOLVER_H_
#define CYCLUS_CYCLOPTS_SOLVER_H_

#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

namespace cyclus {

class Solver;
typedef boost::shared_ptr<Solver> SolverPtr;

} // namespace cyclus

#include "function.h"
#include "variable.h"

namespace cyclus {

/// abstract base class for different types of constraint program solvers
class Solver {
 public:
  /// constructor
  Solver();

  /// virtual destructor
  virtual ~Solver() {};

  /// solve method to be overloaded by derived classes
  virtual void Solve(
      std::vector<Variable::Ptr>& variables, 
      ObjectiveFunction::Ptr obj, 
      std::vector<Constraint::Ptr>& constraints) = 0;

 protected:
  /// the indices used for each variable
  std::map<Variable::Ptr, int> index_;

  /// match variable pointers to indices, populating indicies_
  void PopulateIndices(std::vector<Variable::Ptr>& variables);
};

} // namespace cyclus

#endif
