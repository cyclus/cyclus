#include "variable.h"

#include <string>
#include <sstream>

#include "cyc_limits.h"

// -----------------------------------------------------------------------------
int cyclus::Variable::next_id_ = 0;

// -----------------------------------------------------------------------------
cyclus::Variable::Variable(Bound l, Bound u, VarType t)
    : id_(next_id_++),
      lbound_(l),
      ubound_(u),
      type_(t) {
  std::stringstream ss;
  ss << "var_" << id_;
  name_ = ss.str();
}

// -----------------------------------------------------------------------------
int cyclus::Variable::id() {
  return id_;
}

// -----------------------------------------------------------------------------
cyclus::Variable::VarType cyclus::Variable::type() {
  return type_;
}

// -----------------------------------------------------------------------------
cyclus::Variable::Bound cyclus::Variable::lbound() {
  return lbound_;
}

// -----------------------------------------------------------------------------
cyclus::Variable::Bound cyclus::Variable::ubound() {
  return ubound_;
}

// -----------------------------------------------------------------------------
std::string cyclus::Variable::name() {
  return name_;
}

// -----------------------------------------------------------------------------
void cyclus::Variable::set_name(std::string name) {
  name_ = name;
}

// -----------------------------------------------------------------------------
void cyclus::Variable::set_value(boost::any v) {
  value_ = v;
}

// -----------------------------------------------------------------------------
boost::any cyclus::Variable::value() {
  return value_;
}

// -----------------------------------------------------------------------------
cyclus::LinearVariable::LinearVariable(Bound lb, Bound ub)
    : Variable(lb, ub, LINEAR),
      lbound_val_(-cyclus::kLinBoundLimit),
      ubound_val_(cyclus::kLinBoundLimit) 
{ }

// -----------------------------------------------------------------------------
cyclus::LinearVariable::LinearVariable(double lb_val, Bound ub)
    : Variable(FINITE, ub, LINEAR),
      lbound_val_(lb_val),
      ubound_val_(cyclus::kLinBoundLimit) 
{ }
  
// -----------------------------------------------------------------------------
cyclus::LinearVariable::LinearVariable(Bound lb, double ub_val)
    : Variable(lb, FINITE, LINEAR),
      lbound_val_(-cyclus::kLinBoundLimit),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
cyclus::LinearVariable::LinearVariable(double lb_val, double ub_val)
    : Variable(FINITE, FINITE, LINEAR),
      lbound_val_(lb_val),
      ubound_val_(ub_val) 
{ }

// -----------------------------------------------------------------------------
double cyclus::LinearVariable::lbound_val() {
  return lbound_val_;
}

// -----------------------------------------------------------------------------
double cyclus::LinearVariable::ubound_val() {
  return ubound_val_;
}

// -----------------------------------------------------------------------------
cyclus::IntegerVariable::IntegerVariable(Bound lb, Bound ub)
    : Variable(lb, ub, INT),
      lbound_val_(-cyclus::kIntBoundLimit),
      ubound_val_(cyclus::kIntBoundLimit) 
{ }

// -----------------------------------------------------------------------------
cyclus::IntegerVariable::IntegerVariable(int lb_val, Bound ub)
    : Variable(FINITE, ub, INT),
      lbound_val_(lb_val),
      ubound_val_(cyclus::kIntBoundLimit) 
{ }
  
// -----------------------------------------------------------------------------
cyclus::IntegerVariable::IntegerVariable(Bound lb, int ub_val)
    : Variable(lb, FINITE, INT),
      lbound_val_(-cyclus::kIntBoundLimit),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
cyclus::IntegerVariable::IntegerVariable(int lb_val, int ub_val)
    : Variable(FINITE, FINITE, INT),
      lbound_val_(lb_val),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
int cyclus::IntegerVariable::lbound_val() {
  return lbound_val_;
}

// -----------------------------------------------------------------------------
int cyclus::IntegerVariable::ubound_val() {
  return ubound_val_;
}

// -----------------------------------------------------------------------------
std::pair<int, int> cyclus::GetIntBounds(
    cyclus::VariablePtr v) {
  using cyclus::IntegerVariable;
  IntegerVariable* clone = dynamic_cast<IntegerVariable*>(v.get());
  return std::pair<int, int>(clone->lbound_val(), clone->ubound_val());
}

// -----------------------------------------------------------------------------
std::pair<double, double> cyclus::GetLinBounds(
    cyclus::VariablePtr v) {
  using cyclus::LinearVariable;
  LinearVariable* clone = dynamic_cast<LinearVariable*>(v.get());
  return std::pair<double, double>(clone->lbound_val(), clone->ubound_val());
}
