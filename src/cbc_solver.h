#ifndef CYCLUS_SRC_CBC_SOLVER_H_
#define CYCLUS_SRC_CBC_SOLVER_H_

#include <utility>

// coin includes
#include "CoinModel.hpp"
#include "CbcModel.hpp"

#include "function.h"
#include "solver.h"
#include "variable.h"

typedef CbcModel CoinCbcModel;

namespace cyclus {

/// the coin branch-and-cut solver, see https://projects.coin-or.org/Cbc.
class CBCSolver : public Solver {
 public:
  /// Solve an Mixed-Integer Program.
  /// @param variables A container of variables. The solution is preserved in
  /// the value() member.
  /// @param obj A pointer to the objective function.
  /// @param constraints A container of pointers to the problem's constraints.
  virtual void Solve(std::vector<Variable::Ptr>& variables,
                     ObjectiveFunction::Ptr obj,
                     std::vector<Constraint::Ptr>& constraints);

 private:
  /// the agent builder
  CoinModel builder_;

  /// get coin-specific bound for a constraint
  std::pair<double, double> ConstraintBounds(Constraint::Ptr c);

  /// set variable/objective function values
  void SetUpVariablesAndObj(
      std::vector<Variable::Ptr>& variables,
      ObjectiveFunction::Ptr obj);

  /// set up constraints
  void SetUpConstraints(
      std::vector<Constraint::Ptr>& constraints);

  /// set the objective direction
  double ObjDirection(ObjectiveFunction::Ptr obj);

  /// solve the agent
  void SolveAgent(CbcModel& agent);

  /// populate the solution in the variable vector
  void PopulateSolution(CbcModel& agent,
                        std::vector<Variable::Ptr>& variables);

  /// print variables info
  void PrintVariables(int n_const);

  /// print objective function info
  void PrintObjFunction(int n_vars);

  /// print constraint info
  void PrintConstraints(int n_const, int n_vars);

  /// prints each other printing function
  void Print(int n_const, int n_vars);
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_CBC_SOLVER_H_
