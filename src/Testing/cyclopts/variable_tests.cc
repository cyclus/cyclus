#include "variable_tests.h"

#include <gtest/gtest.h>

#include <string>

#include "boost/any.hpp"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(VariableTests, constructors) {
  cyclus::cyclopts::Variable::Bound lbound(cyclus::cyclopts::Variable::NEG_INF);
  cyclus::cyclopts::Variable::Bound ubound(cyclus::cyclopts::Variable::INF);
  cyclus::cyclopts::Variable::VarType type(cyclus::cyclopts::Variable::LINEAR);
  
  cyclus::cyclopts::Variable var(lbound, ubound, type);
  EXPECT_EQ(var.lbound(), lbound);
  EXPECT_EQ(var.ubound(), ubound);
  EXPECT_EQ(var.type(), type);

  lbound = cyclus::cyclopts::Variable::FINITE;
  ubound = cyclus::cyclopts::Variable::FINITE;
  type = cyclus::cyclopts::Variable::INT;
  var = cyclus::cyclopts::Variable(lbound, ubound, type);
  EXPECT_EQ(var.lbound(), lbound);
  EXPECT_EQ(var.ubound(), ubound);
  EXPECT_EQ(var.type(), type);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(VariableTests, settersgetters) {
  cyclus::cyclopts::Variable::Bound lbound(cyclus::cyclopts::Variable::NEG_INF);
  cyclus::cyclopts::Variable::Bound ubound(cyclus::cyclopts::Variable::INF);
  cyclus::cyclopts::Variable::VarType type(cyclus::cyclopts::Variable::LINEAR);
  
  cyclus::cyclopts::Variable var(lbound, ubound, type);

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
