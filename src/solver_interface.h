#ifndef CYCLUS_CYCLOPTS_SOLVER_INTERFACE_H_
#define CYCLUS_CYCLOPTS_SOLVER_INTERFACE_H_

#include <vector>

#include "function.h"
#include "solver.h"
#include "variable.h"

namespace cyclus {

/// A general interface class for all derived optim solvers to set up and 
/// solve a constraint program.
class SolverInterface {
 public:
  /// constructor
  /// @param solver a pointer to the concrete solver to which an interface will
  /// be provided.
  SolverInterface(Solver::Ptr solver);

  /// add a variable
  /// @param v a pointer to the variable to register with the problem
  void RegisterVariable(Variable::Ptr v);

  /// set the objective function
  /// @param obj a pointer to the objective function to register
  void RegisterObjFunction(ObjectiveFunction::Ptr obj);

  /// add a variable to the objective function
  /// @param v the variable to add to the obj function
  /// @param modifier the modifier for that variable in the obj function
  void AddVarToObjFunction(Variable::Ptr v, double modifier);

  /// add a constraint
  /// @param c a pointer to a constraint function to register
  void RegisterConstraint(Constraint::Ptr c);

  /// add a variable to a constraint
  /// @param v the variable to add to a constraint
  /// @param modifier the modifier for that variable in the constraint
  /// @param c the constraint to add it to
  void AddVarToConstraint(Variable::Ptr v, double modifier, 
                          Constraint::Ptr c);

  /// solve the constraint program
  void Solve();

 private:
  /// the solver
  Solver::Ptr solver_;

  /// the variables
  std::vector<Variable::Ptr> variables_;

  /// the objective function
  ObjectiveFunction::Ptr obj_;

  /// the constraints
  std::vector<Constraint::Ptr> constraints_;

  /// a limit on the modifiers of constraints
  double modifier_limit_;

  /// checks if a modifier is within the acceptable bounds for modifiers
  void CheckModifierBounds(double modifier);
};

} // namespace cyclus

#endif
