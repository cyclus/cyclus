#include "variable_tests.h"

#include <gtest/gtest.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(VariableTests, constructor) {
  cyclopts::Variable::Bound lbound(cyclopts::Variable::NEG_INF);
  cyclopts::Variable::Bound ubound(cyclopts::Variable::FINITE);
  cyclopts::Variable::VarType type(cyclopts::Variable::LINEAR);
  
  cyclopts::Variable var(lbound, ubound, type);
  EXPECT_EQ(var.lbound(), lbound);
  EXPECT_EQ(var.ubound(), ubound);
  EXPECT_EQ(var.type(), type);
}

