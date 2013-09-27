#include "variable.h"

#include <string>
#include <sstream>

#include "cyclopts/limits.h"

// -----------------------------------------------------------------------------
int cyclus::cyclopts::Variable::next_id_ = 0;

// -----------------------------------------------------------------------------
cyclus::cyclopts::Variable::Variable(Bound l, Bound u, VarType t)
    : id_(next_id_++),
      lbound_(l),
      ubound_(u),
      type_(t) {
  std::stringstream ss;
  ss << "var_" << id_;
  name_ = ss.str();
}

// -----------------------------------------------------------------------------
int cyclus::cyclopts::Variable::id() {
  return id_;
}

// -----------------------------------------------------------------------------
cyclus::cyclopts::Variable::VarType cyclus::cyclopts::Variable::type() {
  return type_;
}

// -----------------------------------------------------------------------------
cyclus::cyclopts::Variable::Bound cyclus::cyclopts::Variable::lbound() {
  return lbound_;
}

// -----------------------------------------------------------------------------
cyclus::cyclopts::Variable::Bound cyclus::cyclopts::Variable::ubound() {
  return ubound_;
}

// -----------------------------------------------------------------------------
std::string cyclus::cyclopts::Variable::name() {
  return name_;
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::Variable::set_name(std::string name) {
  name_ = name;
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::Variable::set_value(boost::any v) {
  value_ = v;
}

// -----------------------------------------------------------------------------
boost::any cyclus::cyclopts::Variable::value() {
  return value_;
}

// -----------------------------------------------------------------------------
cyclus::cyclopts::LinearVariable::LinearVariable(Bound lb, Bound ub)
    : Variable(lb, ub, LINEAR),
      lbound_val_(-cyclus::cyclopts::kLinBoundLimit),
      ubound_val_(cyclus::cyclopts::kLinBoundLimit) 
{ }

// -----------------------------------------------------------------------------
cyclus::cyclopts::LinearVariable::LinearVariable(double lb_val, Bound ub)
    : Variable(FINITE, ub, LINEAR),
      lbound_val_(lb_val),
      ubound_val_(cyclus::cyclopts::kLinBoundLimit) 
{ }
  
// -----------------------------------------------------------------------------
cyclus::cyclopts::LinearVariable::LinearVariable(Bound lb, double ub_val)
    : Variable(lb, FINITE, LINEAR),
      lbound_val_(-cyclus::cyclopts::kLinBoundLimit),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
cyclus::cyclopts::LinearVariable::LinearVariable(double lb_val, double ub_val)
    : Variable(FINITE, FINITE, LINEAR),
      lbound_val_(lb_val),
      ubound_val_(ub_val) 
{ }

// -----------------------------------------------------------------------------
double cyclus::cyclopts::LinearVariable::lbound_val() {
  return lbound_val_;
}

// -----------------------------------------------------------------------------
double cyclus::cyclopts::LinearVariable::ubound_val() {
  return ubound_val_;
}

// -----------------------------------------------------------------------------
cyclus::cyclopts::IntegerVariable::IntegerVariable(Bound lb, Bound ub)
    : Variable(lb, ub, INT),
      lbound_val_(-cyclus::cyclopts::kIntBoundLimit),
      ubound_val_(cyclus::cyclopts::kIntBoundLimit) 
{ }

// -----------------------------------------------------------------------------
cyclus::cyclopts::IntegerVariable::IntegerVariable(int lb_val, Bound ub)
    : Variable(FINITE, ub, INT),
      lbound_val_(lb_val),
      ubound_val_(cyclus::cyclopts::kIntBoundLimit) 
{ }
  
// -----------------------------------------------------------------------------
cyclus::cyclopts::IntegerVariable::IntegerVariable(Bound lb, int ub_val)
    : Variable(lb, FINITE, INT),
      lbound_val_(-cyclus::cyclopts::kIntBoundLimit),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
cyclus::cyclopts::IntegerVariable::IntegerVariable(int lb_val, int ub_val)
    : Variable(FINITE, FINITE, INT),
      lbound_val_(lb_val),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
int cyclus::cyclopts::IntegerVariable::lbound_val() {
  return lbound_val_;
}

// -----------------------------------------------------------------------------
int cyclus::cyclopts::IntegerVariable::ubound_val() {
  return ubound_val_;
}

// -----------------------------------------------------------------------------
std::pair<int, int> cyclus::cyclopts::GetIntBounds(
    cyclus::cyclopts::VariablePtr v) {
  using cyclus::cyclopts::IntegerVariable;
  IntegerVariable* clone = dynamic_cast<IntegerVariable*>(v.get());
  return std::pair<int, int>(clone->lbound_val(), clone->ubound_val());
}

// -----------------------------------------------------------------------------
std::pair<double, double> cyclus::cyclopts::GetLinBounds(
    cyclus::cyclopts::VariablePtr v) {
  using cyclus::cyclopts::LinearVariable;
  LinearVariable* clone = dynamic_cast<LinearVariable*>(v.get());
  return std::pair<double, double>(clone->lbound_val(), clone->ubound_val());
}
