#include "optim/function.h"

#include <stdlib.h>

#include <gtest/gtest.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsFunctionTests, AddVars) {
  // usings
  using cyclus::optim::Variable;
  using cyclus::optim::VariablePtr;
  using cyclus::optim::Function;

  Variable::Bound lbound(Variable::NEG_INF);
  Variable::Bound ubound(Variable::INF);
  Variable::VarType type(Variable::LINEAR);
  
  double val1 = 1.5, val2 = 2.3;
  VariablePtr var1(new Variable(lbound, ubound, type));
  VariablePtr var2(new Variable(lbound, ubound, type));

  Function f;
  f.AddVariable(var1, val1);
  EXPECT_EQ(1, f.NumVars());
  f.AddVariable(var2, val2);
  EXPECT_EQ(2, f.NumVars());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsFunctionTests, GetModifiers) {
  // usings
  using cyclus::optim::Variable;
  using cyclus::optim::VariablePtr;
  using cyclus::optim::Function;

  Variable::Bound lbound(Variable::NEG_INF);
  Variable::Bound ubound(Variable::INF);
  Variable::VarType type(Variable::LINEAR);
  
  double val1 = 1.5;
  VariablePtr var1(new Variable(lbound, ubound, type));
  Function f;
  f.AddVariable(var1, val1);

  EXPECT_EQ(val1, f.GetModifier(var1));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsObjectiveFunctionTests, Constructor) {
  // usings
  using cyclus::optim::ObjectiveFunction;

  ObjectiveFunction::Direction dir;

  dir = ObjectiveFunction::MIN;
  ObjectiveFunction o(dir);
  EXPECT_EQ(dir, o.dir());
  
  dir = ObjectiveFunction::MAX;
  o = ObjectiveFunction(dir);
  EXPECT_EQ(dir, o.dir());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// class for parameterized constraint function testing
class ConstraintTests :
    public
::testing::TestWithParam<cyclus::optim::Constraint::EqualityRelation> { };

// parameterized test
TEST_P(ConstraintTests, Constructor) {
  // usings
  using cyclus::optim::Constraint;

  Constraint::EqualityRelation eq_r = GetParam();
  double rhs = static_cast<double>(std::rand());
  Constraint c(eq_r, rhs);

  EXPECT_EQ(eq_r, c.eq_relation());
  EXPECT_EQ(rhs, c.rhs());
}

// call test
const cyclus::optim::Constraint::EqualityRelation relations[] = {
  cyclus::optim::Constraint::EQ,
  cyclus::optim::Constraint::GT,
  cyclus::optim::Constraint::GTEQ,
  cyclus::optim::Constraint::LT,
  cyclus::optim::Constraint::LTEQ
};

INSTANTIATE_TEST_CASE_P(CycloptsConstraintConstructor, ConstraintTests,
                        ::testing::ValuesIn(relations));
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
