#ifndef CYCLOPTS_SOLVER_INTERFACE_H_
#define CYCLOPTS_SOLVER_INTERFACE_H_

#include <vector>

#include "function.h"
#include "solver.h"
#include "variable.h"

namespace cyclopts {
/// interface class to set up and solve a constraint program
class SolverInterface {
 public:
  /// constructor, sets the solver to use
  SolverInterface(SolverPtr solver);

  /// add a constraint
  void RegisterVariable(VariablePtr v);

  /// set the objective function
  void RegisterObjFunction(ObjFuncPtr obj);

  /// add a variable to the objective function
  void AddVarToObjFunction(VariablePtr v, double modifier);

  /// add a constraint
  void RegisterConstraint(ConstraintPtr c);

  /// add a variable to a constraint
  void AddVarToConstraint(VariablePtr v, double modifier, ConstraintPtr c);

  /// solve the constraint program
  void Solve();

 private:
  /// the solver
  SolverPtr solver_;

  /// the variables
  std::vector<VariablePtr> variables_;

  /// the objective function
  ObjFuncPtr obj_;

  /// the constraints
  std::vector<ConstraintPtr> constraints_;

  /// a limit on the modifiers of constraints
  double modifier_limit_;

  /// checks if a modifier is within the acceptable bounds for modifiers
  void CheckModifierBounds(double modifier);
};
}

#endif
