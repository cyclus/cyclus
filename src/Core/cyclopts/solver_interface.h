#ifndef CYCLOPTS_SOLVER_INTERFACE_H_
#define CYCLOPTS_SOLVER_INTERFACE_H_

#include <vector>

#include "function.h"
#include "solver.h"
#include "variable.h"

namespace cyclus {
namespace cyclopts {
/// interface class to set up and solve a constraint program
class SolverInterface {
 public:
  /// constructor, sets the solver to use
  SolverInterface(SolverPtr solver);

  /// add a constraint
  void RegisterVariable(cyclus::cyclopts::VariablePtr v);

  /// set the objective function
  void RegisterObjFunction(cyclus::cyclopts::ObjFuncPtr obj);

  /// add a variable to the objective function
  void AddVarToObjFunction(cyclus::cyclopts::VariablePtr v, double modifier);

  /// add a constraint
  void RegisterConstraint(cyclus::cyclopts::ConstraintPtr c);

  /// add a variable to a constraint
  void AddVarToConstraint(cyclus::cyclopts::VariablePtr v, double modifier, cyclus::cyclopts::ConstraintPtr c);

  /// solve the constraint program
  void Solve();

 private:
  /// the solver
  SolverPtr solver_;

  /// the variables
  std::vector<cyclus::cyclopts::VariablePtr> variables_;

  /// the objective function
  cyclus::cyclopts::ObjFuncPtr obj_;

  /// the constraints
  std::vector<cyclus::cyclopts::ConstraintPtr> constraints_;

  /// a limit on the modifiers of constraints
  double modifier_limit_;

  /// checks if a modifier is within the acceptable bounds for modifiers
  void CheckModifierBounds(double modifier);
};
} // namespace cyclopts
} // namespace cyclus

#endif
