#include "variable.h"

#include <string>
#include <sstream>

#include "optim/limits.h"

// -----------------------------------------------------------------------------
int cyclus::optim::Variable::next_id_ = 0;

// -----------------------------------------------------------------------------
cyclus::optim::Variable::Variable(Bound l, Bound u, VarType t)
    : id_(next_id_++),
      lbound_(l),
      ubound_(u),
      type_(t) {
  std::stringstream ss;
  ss << "var_" << id_;
  name_ = ss.str();
}

// -----------------------------------------------------------------------------
int cyclus::optim::Variable::id() {
  return id_;
}

// -----------------------------------------------------------------------------
cyclus::optim::Variable::VarType cyclus::optim::Variable::type() {
  return type_;
}

// -----------------------------------------------------------------------------
cyclus::optim::Variable::Bound cyclus::optim::Variable::lbound() {
  return lbound_;
}

// -----------------------------------------------------------------------------
cyclus::optim::Variable::Bound cyclus::optim::Variable::ubound() {
  return ubound_;
}

// -----------------------------------------------------------------------------
std::string cyclus::optim::Variable::name() {
  return name_;
}

// -----------------------------------------------------------------------------
void cyclus::optim::Variable::set_name(std::string name) {
  name_ = name;
}

// -----------------------------------------------------------------------------
void cyclus::optim::Variable::set_value(boost::any v) {
  value_ = v;
}

// -----------------------------------------------------------------------------
boost::any cyclus::optim::Variable::value() {
  return value_;
}

// -----------------------------------------------------------------------------
cyclus::optim::LinearVariable::LinearVariable(Bound lb, Bound ub)
    : Variable(lb, ub, LINEAR),
      lbound_val_(-cyclus::optim::kLinBoundLimit),
      ubound_val_(cyclus::optim::kLinBoundLimit) 
{ }

// -----------------------------------------------------------------------------
cyclus::optim::LinearVariable::LinearVariable(double lb_val, Bound ub)
    : Variable(FINITE, ub, LINEAR),
      lbound_val_(lb_val),
      ubound_val_(cyclus::optim::kLinBoundLimit) 
{ }
  
// -----------------------------------------------------------------------------
cyclus::optim::LinearVariable::LinearVariable(Bound lb, double ub_val)
    : Variable(lb, FINITE, LINEAR),
      lbound_val_(-cyclus::optim::kLinBoundLimit),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
cyclus::optim::LinearVariable::LinearVariable(double lb_val, double ub_val)
    : Variable(FINITE, FINITE, LINEAR),
      lbound_val_(lb_val),
      ubound_val_(ub_val) 
{ }

// -----------------------------------------------------------------------------
double cyclus::optim::LinearVariable::lbound_val() {
  return lbound_val_;
}

// -----------------------------------------------------------------------------
double cyclus::optim::LinearVariable::ubound_val() {
  return ubound_val_;
}

// -----------------------------------------------------------------------------
cyclus::optim::IntegerVariable::IntegerVariable(Bound lb, Bound ub)
    : Variable(lb, ub, INT),
      lbound_val_(-cyclus::optim::kIntBoundLimit),
      ubound_val_(cyclus::optim::kIntBoundLimit) 
{ }

// -----------------------------------------------------------------------------
cyclus::optim::IntegerVariable::IntegerVariable(int lb_val, Bound ub)
    : Variable(FINITE, ub, INT),
      lbound_val_(lb_val),
      ubound_val_(cyclus::optim::kIntBoundLimit) 
{ }
  
// -----------------------------------------------------------------------------
cyclus::optim::IntegerVariable::IntegerVariable(Bound lb, int ub_val)
    : Variable(lb, FINITE, INT),
      lbound_val_(-cyclus::optim::kIntBoundLimit),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
cyclus::optim::IntegerVariable::IntegerVariable(int lb_val, int ub_val)
    : Variable(FINITE, FINITE, INT),
      lbound_val_(lb_val),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
int cyclus::optim::IntegerVariable::lbound_val() {
  return lbound_val_;
}

// -----------------------------------------------------------------------------
int cyclus::optim::IntegerVariable::ubound_val() {
  return ubound_val_;
}

// -----------------------------------------------------------------------------
std::pair<int, int> cyclus::optim::GetIntBounds(
    cyclus::optim::VariablePtr v) {
  using cyclus::optim::IntegerVariable;
  IntegerVariable* clone = dynamic_cast<IntegerVariable*>(v.get());
  return std::pair<int, int>(clone->lbound_val(), clone->ubound_val());
}

// -----------------------------------------------------------------------------
std::pair<double, double> cyclus::optim::GetLinBounds(
    cyclus::optim::VariablePtr v) {
  using cyclus::optim::LinearVariable;
  LinearVariable* clone = dynamic_cast<LinearVariable*>(v.get());
  return std::pair<double, double>(clone->lbound_val(), clone->ubound_val());
}
