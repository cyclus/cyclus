#include "variable_tests.h"

#include <gtest/gtest.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(VariableTests, constructor) {
  cyclus::cyclopts::Variable::Bound lbound(cyclus::cyclopts::Variable::NEG_INF);
  cyclus::cyclopts::Variable::Bound ubound(cyclus::cyclopts::Variable::FINITE);
  cyclus::cyclopts::Variable::VarType type(cyclus::cyclopts::Variable::LINEAR);
  
  cyclus::cyclopts::Variable var(lbound, ubound, type);
  EXPECT_EQ(var.lbound(), lbound);
  EXPECT_EQ(var.ubound(), ubound);
  EXPECT_EQ(var.type(), type);
}

