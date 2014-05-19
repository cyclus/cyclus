#include <stdlib.h>

#include <gtest/gtest.h>

#include "toolkit/function.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsFunctionTests, AddVars) {

  Variable::Bound lbound(Variable::NEG_INF);
  Variable::Bound ubound(Variable::INF);
  Variable::VarType type(Variable::LINEAR);

  double val1 = 1.5, val2 = 2.3;
  Variable::Ptr var1(new Variable(lbound, ubound, type));
  Variable::Ptr var2(new Variable(lbound, ubound, type));

  Function f;
  f.AddVariable(var1, val1);
  EXPECT_EQ(1, f.NumVars());
  f.AddVariable(var2, val2);
  EXPECT_EQ(2, f.NumVars());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsFunctionTests, GetModifiers) {

  Variable::Bound lbound(Variable::NEG_INF);
  Variable::Bound ubound(Variable::INF);
  Variable::VarType type(Variable::LINEAR);

  double val1 = 1.5;
  Variable::Ptr var1(new Variable(lbound, ubound, type));
  Function f;
  f.AddVariable(var1, val1);

  EXPECT_EQ(val1, f.GetModifier(var1));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsObjectiveFunctionTests, Constructor) {

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
::testing::TestWithParam<Constraint::EqualityRelation> {};

// parameterized test
TEST_P(ConstraintTests, Constructor) {
  Constraint::EqualityRelation eq_r = GetParam();
  double rhs = static_cast<double>(std::rand());
  Constraint c(eq_r, rhs);

  EXPECT_EQ(eq_r, c.eq_relation());
  EXPECT_EQ(rhs, c.rhs());
}

// call test
const Constraint::EqualityRelation relations[] = {
  Constraint::EQ,
  Constraint::GT,
  Constraint::GTEQ,
  Constraint::LT,
  Constraint::LTEQ
};

INSTANTIATE_TEST_CASE_P(CycloptsConstraintConstructor, ConstraintTests,
                        ::testing::ValuesIn(relations));
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
}
