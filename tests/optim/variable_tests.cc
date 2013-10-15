#include "optim/variable.h"

#include <gtest/gtest.h>

#include <stdlib.h>
#include <utility>

#include "boost/any.hpp"

#include "optim/limits.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsVariableTests, UnboundedLinearConstructors) {
  // usings
  using cyclus::optim::Variable;
  using cyclus::optim::LinearVariable;

  Variable::Bound lbound(Variable::NEG_INF);
  Variable::Bound ubound(Variable::INF);
  Variable::VarType type(Variable::LINEAR);
  
  LinearVariable var(lbound, ubound);
  EXPECT_EQ(var.lbound(), lbound);
  EXPECT_EQ(var.ubound(), ubound);
  EXPECT_EQ(var.type(), type);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsVariableTests, BoundedLinearConstructors) {
  // usings
  using cyclus::optim::Variable;
  using cyclus::optim::VariablePtr;
  using cyclus::optim::LinearVariable;

  Variable::Bound neg_inf(Variable::NEG_INF);
  Variable::Bound inf(Variable::INF);
  Variable::Bound finite(Variable::FINITE);
  Variable::VarType type(Variable::LINEAR);
  double lbound_limit = -cyclus::optim::kLinBoundLimit;
  double ubound_limit = cyclus::optim::kLinBoundLimit;
  double val = static_cast<double>(std::rand());
  
  LinearVariable var1(neg_inf, val);
  EXPECT_EQ(var1.lbound(), neg_inf);
  EXPECT_EQ(var1.lbound_val(), lbound_limit);
  EXPECT_EQ(var1.ubound(), finite);
  EXPECT_EQ(var1.ubound_val(), val);
  EXPECT_EQ(var1.type(), type);
  std::pair<double, double> bounds(lbound_limit, val);
  EXPECT_EQ(bounds, GetLinBounds(VariablePtr(new LinearVariable(var1))));
  
  LinearVariable var2(val, inf);
  EXPECT_EQ(var2.lbound(), finite);
  EXPECT_EQ(var2.lbound_val(), val);
  EXPECT_EQ(var2.ubound(), inf);
  EXPECT_EQ(var2.ubound_val(), ubound_limit);
  EXPECT_EQ(var2.type(), type);
  bounds = std::pair<double, double>(val, ubound_limit);
  EXPECT_EQ(bounds, GetLinBounds(VariablePtr(new LinearVariable(var2))));

  LinearVariable var3(val, val);
  EXPECT_EQ(var3.lbound(), finite);
  EXPECT_EQ(var3.lbound_val(), val);
  EXPECT_EQ(var3.ubound(), finite);
  EXPECT_EQ(var3.ubound_val(), val);
  EXPECT_EQ(var3.type(), type);
  bounds = std::pair<double, double>(val, val);
  EXPECT_EQ(bounds, GetLinBounds(VariablePtr(new LinearVariable(var3))));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsVariableTests, UnboundedIntConstructors) {
  // usings
  using cyclus::optim::Variable;
  using cyclus::optim::IntegerVariable;

  Variable::Bound lbound(Variable::NEG_INF);
  Variable::Bound ubound(Variable::INF);
  Variable::VarType type(Variable::INT);
  
  IntegerVariable var(lbound, ubound);
  EXPECT_EQ(var.lbound(), lbound);
  EXPECT_EQ(var.ubound(), ubound);
  EXPECT_EQ(var.type(), type);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsVariableTests, BoundedIntegerConstructors) {
  // usings
  using cyclus::optim::Variable;
  using cyclus::optim::VariablePtr;
  using cyclus::optim::IntegerVariable;

  Variable::Bound neg_inf(Variable::NEG_INF);
  Variable::Bound inf(Variable::INF);
  Variable::Bound finite(Variable::FINITE);
  Variable::VarType type(Variable::INT);
  double lbound_limit = -cyclus::optim::kIntBoundLimit;
  double ubound_limit = cyclus::optim::kIntBoundLimit;
  double val = std::rand();
  
  IntegerVariable var1(neg_inf, val);
  EXPECT_EQ(var1.lbound(), neg_inf);
  EXPECT_EQ(var1.lbound_val(), lbound_limit);
  EXPECT_EQ(var1.ubound(), finite);
  EXPECT_EQ(var1.ubound_val(), val);
  EXPECT_EQ(var1.type(), type);
  std::pair<int, int> bounds(lbound_limit, val);
  EXPECT_EQ(bounds, GetIntBounds(VariablePtr(new IntegerVariable(var1))));
  
  IntegerVariable var2(val, inf);
  EXPECT_EQ(var2.lbound(), finite);
  EXPECT_EQ(var2.lbound_val(), val);
  EXPECT_EQ(var2.ubound(), inf);
  EXPECT_EQ(var2.ubound_val(), ubound_limit);
  EXPECT_EQ(var2.type(), type);
  bounds = std::pair<int, int>(val, ubound_limit);
  EXPECT_EQ(bounds, GetIntBounds(VariablePtr(new IntegerVariable(var2))));
  
  IntegerVariable var3(val, val);
  EXPECT_EQ(var3.lbound(), finite);
  EXPECT_EQ(var3.lbound_val(), val);
  EXPECT_EQ(var3.ubound(), finite);
  EXPECT_EQ(var3.ubound_val(), val);
  EXPECT_EQ(var3.type(), type);
  bounds = std::pair<int, int>(val, val);
  EXPECT_EQ(bounds, GetIntBounds(VariablePtr(new IntegerVariable(var3))));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsVariableTests, LinearSettersGetters) {
  // usings
  using cyclus::optim::Variable;
  using cyclus::optim::LinearVariable;

  Variable::Bound lbound(Variable::NEG_INF);
  Variable::Bound ubound(Variable::INF);
  
  LinearVariable var(lbound, ubound);

  std::string name = "name";
  var.set_name(name);
  EXPECT_EQ(name, var.name());

  boost::any value;
  value = 1.5;
  var.set_value(value);
  EXPECT_EQ(boost::any_cast<double>(value), boost::any_cast<double>(var.value()));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CycloptsVariableTests, IntegerSettersGetters) {
  // usings
  using cyclus::optim::Variable;
  using cyclus::optim::IntegerVariable;

  Variable::Bound lbound(Variable::NEG_INF);
  Variable::Bound ubound(Variable::INF);
  
  IntegerVariable var(lbound, ubound);

  std::string name = "name";
  var.set_name(name);
  EXPECT_EQ(name, var.name());

  boost::any value;
  value = 1;
  var.set_value(value);
  EXPECT_EQ(boost::any_cast<int>(value), boost::any_cast<int>(var.value()));
}
