#include "variable_tests.h"

#include <gtest/gtest.h>

#include <string>

#include "boost/any.hpp"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsVariableTests, constructors) {
  // usings
  using cyclus::cyclopts::Variable;

  Variable::Bound lbound(Variable::NEG_INF);
  Variable::Bound ubound(Variable::INF);
  Variable::VarType type(Variable::LINEAR);
  
  Variable var(lbound, ubound, type);
  EXPECT_EQ(var.lbound(), lbound);
  EXPECT_EQ(var.ubound(), ubound);
  EXPECT_EQ(var.type(), type);

  lbound = Variable::FINITE;
  ubound = Variable::FINITE;
  type = Variable::INT;
  var = Variable(lbound, ubound, type);
  EXPECT_EQ(var.lbound(), lbound);
  EXPECT_EQ(var.ubound(), ubound);
  EXPECT_EQ(var.type(), type);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsVariableTests, settersgetters) {
  // usings
  using cyclus::cyclopts::Variable;

  Variable::Bound lbound(Variable::NEG_INF);
  Variable::Bound ubound(Variable::INF);
  Variable::VarType type(Variable::LINEAR);
  
  Variable var(lbound, ubound, type);

  std::string name = "name";
  var.set_name(name);
  EXPECT_EQ(name, var.name());

  boost::any value(1);
  var.set_value(value);
  EXPECT_EQ(boost::any_cast<int>(value), boost::any_cast<int>(var.value()));

  value = 1.5;
  var.set_value(value);
  EXPECT_EQ(boost::any_cast<double>(value), boost::any_cast<double>(var.value()));
}
