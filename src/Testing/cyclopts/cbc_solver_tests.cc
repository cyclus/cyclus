
#include <gtest/gtest.h>

#include <boost/any.hpp>

#include "cyclopts/cbc_solver.h"
#include "cyclopts/function.h"
#include "cyclopts/solver.h"
#include "cyclopts/solver_interface.h"
#include "cyclopts/variable.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsCBCSolverTests, 1VarIP) {
  // usings
  using boost::any_cast;
  using cyclus::cyclopts::SolverPtr;
  using cyclus::cyclopts::CBCSolver;
  using cyclus::cyclopts::Constraint;
  using cyclus::cyclopts::ConstraintPtr;
  using cyclus::cyclopts::ObjFuncPtr;
  using cyclus::cyclopts::SolverInterface;
  using cyclus::cyclopts::Variable;
  using cyclus::cyclopts::VariablePtr;
  using cyclus::cyclopts::IntegerVariable;
  using cyclus::cyclopts::ObjectiveFunction;

  // problem instance values
  int x_exp = 1;
  int upper = 1;
  int lower = 0;
  double obj_mod = 1.0;
  
  // set up solver and interface
  SolverPtr solver(new CBCSolver());
  SolverInterface csi(solver);

  // set up objective function
  ObjFuncPtr obj(new ObjectiveFunction(ObjectiveFunction::MAX));
  csi.RegisterObjFunction(obj);

  // set up variables
  VariablePtr x(new IntegerVariable(lower, upper));
  csi.RegisterVariable(x);

  // objective function
  csi.AddVarToObjFunction(x, obj_mod);
  
  // solve and get solution
  csi.Solve();

  // check
  EXPECT_EQ(x_exp, any_cast<int>(x->value()));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsCBCSolverTests, 2VarIP) {
  // usings
  using boost::any_cast;
  using cyclus::cyclopts::SolverPtr;
  using cyclus::cyclopts::CBCSolver;
  using cyclus::cyclopts::Constraint;
  using cyclus::cyclopts::ConstraintPtr;
  using cyclus::cyclopts::ObjFuncPtr;
  using cyclus::cyclopts::SolverInterface;
  using cyclus::cyclopts::Variable;
  using cyclus::cyclopts::VariablePtr;
  using cyclus::cyclopts::IntegerVariable;
  using cyclus::cyclopts::ObjectiveFunction;

  // problem instance values
  int x_exp = 1, y_exp = 2;
  double cap_x = 3.0, cap_y = 10.0, cost_x = 1.0, cost_y = 2.0;
  double unmet_demand = 22.0;
  
  // set up solver and interface
  SolverPtr solver(new CBCSolver());
  SolverInterface csi(solver);

  // set up objective function
  ObjFuncPtr obj(new ObjectiveFunction(ObjectiveFunction::MIN));
  csi.RegisterObjFunction(obj);

  // set up constraint
  ConstraintPtr c(new Constraint(Constraint::GTEQ, unmet_demand));
  csi.RegisterConstraint(c);

  // set up variables
  VariablePtr x(new IntegerVariable(0, Variable::INF));
  csi.RegisterVariable(x);
  VariablePtr y(new IntegerVariable(0, Variable::INF));
  csi.RegisterVariable(y);

  // configure constraint and objective function
  csi.AddVarToConstraint(x, cap_x, c);
  csi.AddVarToConstraint(y, cap_y, c);
  csi.AddVarToObjFunction(x, cost_x);
  csi.AddVarToObjFunction(y, cost_y);
  
  // solve and get solution
  csi.Solve();

  // check
  EXPECT_EQ(x_exp, any_cast<int>(x->value()));
  EXPECT_EQ(y_exp, any_cast<int>(y->value()));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsCBCSolverTests, 2VarIPBounded) {
  // usings
  using boost::any_cast;
  using cyclus::cyclopts::SolverPtr;
  using cyclus::cyclopts::CBCSolver;
  using cyclus::cyclopts::Constraint;
  using cyclus::cyclopts::ConstraintPtr;
  using cyclus::cyclopts::ObjFuncPtr;
  using cyclus::cyclopts::SolverInterface;
  using cyclus::cyclopts::Variable;
  using cyclus::cyclopts::VariablePtr;
  using cyclus::cyclopts::IntegerVariable;
  using cyclus::cyclopts::ObjectiveFunction;

  // problem instance values
  int x_exp = 1, y_exp = 2;
  double cap_x = 3.0, cap_y = 10.0, cost_x = 1.0, cost_y = 2.0;
  double unmet_demand = 22.0;
  int upper = 5;
  int lower = 0;
  
  // set up solver and interface
  SolverPtr solver(new CBCSolver());
  SolverInterface csi(solver);

  // set up objective function
  ObjFuncPtr obj(new ObjectiveFunction(ObjectiveFunction::MIN));
  csi.RegisterObjFunction(obj);

  // set up constraint
  ConstraintPtr c(new Constraint(Constraint::GTEQ, unmet_demand));
  csi.RegisterConstraint(c);

  // set up variables
  VariablePtr x(new IntegerVariable(lower, upper));
  csi.RegisterVariable(x);
  VariablePtr y(new IntegerVariable(lower, upper));
  csi.RegisterVariable(y);

  // configure constraint and objective function
  csi.AddVarToConstraint(x, cap_x, c);
  csi.AddVarToConstraint(y, cap_y, c);
  csi.AddVarToObjFunction(x, cost_x);
  csi.AddVarToObjFunction(y, cost_y);
  
  // solve and get solution
  csi.Solve();

  // check
  EXPECT_EQ(x_exp, any_cast<int>(x->value()));
  EXPECT_EQ(y_exp, any_cast<int>(y->value()));
}
