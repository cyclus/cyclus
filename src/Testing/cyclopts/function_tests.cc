#include "cyclopts/function.h"

#include <gtest/gtest.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsFunctionTests, AddVars) {
  // usings
  using cyclus::cyclopts::Variable;
  using cyclus::cyclopts::VariablePtr;
  using cyclus::cyclopts::Function;

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
  using cyclus::cyclopts::Variable;
  using cyclus::cyclopts::VariablePtr;
  using cyclus::cyclopts::Function;

  Variable::Bound lbound(Variable::NEG_INF);
  Variable::Bound ubound(Variable::INF);
  Variable::VarType type(Variable::LINEAR);
  
  double val1 = 1.5;
  VariablePtr var1(new Variable(lbound, ubound, type));
  Function f;
  f.AddVariable(var1, val1);

  EXPECT_EQ(val1, f.GetModifier(var1));
}
