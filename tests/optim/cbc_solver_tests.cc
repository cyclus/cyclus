
#include <gtest/gtest.h>

#include <boost/any.hpp>

#include "CoinModel.hpp"

#include "optim/cbc_solver.h"
#include "optim/function.h"
#include "optim/solver.h"
#include "optim/solver_interface.h"
#include "optim/variable.h"
#include "optim/limits.h"

// usings
using boost::any_cast;
using cyclus::optim::SolverPtr;
using cyclus::optim::CBCSolver;
using cyclus::optim::Constraint;
using cyclus::optim::ConstraintPtr;
using cyclus::optim::ObjFuncPtr;
using cyclus::optim::SolverInterface;
using cyclus::optim::Variable;
using cyclus::optim::VariablePtr;
using cyclus::optim::IntegerVariable;
using cyclus::optim::ObjectiveFunction;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsCBCSolverTests, 1VarIPLowerBoundMin) {
  // problem instance values
  int x_exp = 0;
  int lower = 0;
  double obj_mod = 1.0;
  
  // set up solver and interface
  SolverPtr solver(new CBCSolver());
  SolverInterface csi(solver);

  // set up objective function
  ObjFuncPtr obj(new ObjectiveFunction(ObjectiveFunction::MIN));
  csi.RegisterObjFunction(obj);

  // set up variables
  VariablePtr x(new IntegerVariable(lower, Variable::INF));
  csi.RegisterVariable(x);

  // objective function
  csi.AddVarToObjFunction(x, obj_mod);
  
  // solve and get solution
  csi.Solve();

  // check
  EXPECT_EQ(x_exp, any_cast<int>(x->value()));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsCBCSolverTests, 1VarIPBothBoundsMin) {
  // problem instance values
  int x_exp = 0;
  int upper = 1;
  int lower = 0;
  double obj_mod = 1.0;
  
  // set up solver and interface
  SolverPtr solver(new CBCSolver());
  SolverInterface csi(solver);

  // set up objective function
  ObjFuncPtr obj(new ObjectiveFunction(ObjectiveFunction::MIN));
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
TEST(CycloptsCBCSolverTests, 1VarIPUpperBoundMax) {
  // problem instance values
  int x_exp = 1;
  int upper = 1;
  double obj_mod = 1.0;
  
  // set up solver and interface
  SolverPtr solver(new CBCSolver());
  SolverInterface csi(solver);

  // set up objective function
  ObjFuncPtr obj(new ObjectiveFunction(ObjectiveFunction::MAX));
  csi.RegisterObjFunction(obj);

  // set up variables
  VariablePtr x(new IntegerVariable(Variable::NEG_INF, upper));
  csi.RegisterVariable(x);

  // objective function
  csi.AddVarToObjFunction(x, obj_mod);
  
  // solve and get solution
  csi.Solve();

  // check
  EXPECT_EQ(x_exp, any_cast<int>(x->value()));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsCBCSolverTests, 1VarIPBothBoundsMax) {
  // problem instance values
  int x_exp = 1;
  int lower = 0;
  int upper = 1;
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
  using cyclus::optim::SolverPtr;
  using cyclus::optim::CBCSolver;
  using cyclus::optim::Constraint;
  using cyclus::optim::ConstraintPtr;
  using cyclus::optim::ObjFuncPtr;
  using cyclus::optim::SolverInterface;
  using cyclus::optim::Variable;
  using cyclus::optim::VariablePtr;
  using cyclus::optim::IntegerVariable;
  using cyclus::optim::ObjectiveFunction;

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
