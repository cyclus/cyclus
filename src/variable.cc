#include "variable.h"

#include <string>
#include <sstream>

#include "cyc_limits.h"

namespace cyclus {

// -----------------------------------------------------------------------------
int Variable::next_id_ = 0;

// -----------------------------------------------------------------------------
Variable::Variable(Bound l, Bound u, VarType t)
    : id_(next_id_++),
      lbound_(l),
      ubound_(u),
      type_(t) {
  std::stringstream ss;
  ss << "var_" << id_;
  name_ = ss.str();
}

// -----------------------------------------------------------------------------
int Variable::id() {
  return id_;
}

// -----------------------------------------------------------------------------
Variable::VarType Variable::type() {
  return type_;
}

// -----------------------------------------------------------------------------
Variable::Bound Variable::lbound() {
  return lbound_;
}

// -----------------------------------------------------------------------------
Variable::Bound Variable::ubound() {
  return ubound_;
}

// -----------------------------------------------------------------------------
std::string Variable::name() {
  return name_;
}

// -----------------------------------------------------------------------------
void Variable::set_name(std::string name) {
  name_ = name;
}

// -----------------------------------------------------------------------------
void Variable::set_value(boost::any v) {
  value_ = v;
}

// -----------------------------------------------------------------------------
boost::any Variable::value() {
  return value_;
}

// -----------------------------------------------------------------------------
LinearVariable::LinearVariable(Bound lb, Bound ub)
    : Variable(lb, ub, LINEAR),
      lbound_val_(-kLinBoundLimit),
      ubound_val_(kLinBoundLimit) 
{ }

// -----------------------------------------------------------------------------
LinearVariable::LinearVariable(double lb_val, Bound ub)
    : Variable(FINITE, ub, LINEAR),
      lbound_val_(lb_val),
      ubound_val_(kLinBoundLimit) 
{ }
  
// -----------------------------------------------------------------------------
LinearVariable::LinearVariable(Bound lb, double ub_val)
    : Variable(lb, FINITE, LINEAR),
      lbound_val_(-kLinBoundLimit),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
LinearVariable::LinearVariable(double lb_val, double ub_val)
    : Variable(FINITE, FINITE, LINEAR),
      lbound_val_(lb_val),
      ubound_val_(ub_val) 
{ }

// -----------------------------------------------------------------------------
double LinearVariable::lbound_val() {
  return lbound_val_;
}

// -----------------------------------------------------------------------------
double LinearVariable::ubound_val() {
  return ubound_val_;
}

// -----------------------------------------------------------------------------
IntegerVariable::IntegerVariable(Bound lb, Bound ub)
    : Variable(lb, ub, INT),
      lbound_val_(-kIntBoundLimit),
      ubound_val_(kIntBoundLimit) 
{ }

// -----------------------------------------------------------------------------
IntegerVariable::IntegerVariable(int lb_val, Bound ub)
    : Variable(FINITE, ub, INT),
      lbound_val_(lb_val),
      ubound_val_(kIntBoundLimit) 
{ }
  
// -----------------------------------------------------------------------------
IntegerVariable::IntegerVariable(Bound lb, int ub_val)
    : Variable(lb, FINITE, INT),
      lbound_val_(-kIntBoundLimit),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
IntegerVariable::IntegerVariable(int lb_val, int ub_val)
    : Variable(FINITE, FINITE, INT),
      lbound_val_(lb_val),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
int IntegerVariable::lbound_val() {
  return lbound_val_;
}

// -----------------------------------------------------------------------------
int IntegerVariable::ubound_val() {
  return ubound_val_;
}

// -----------------------------------------------------------------------------
std::pair<int, int> GetIntBounds(VariablePtr v) {
  IntegerVariable* clone = dynamic_cast<IntegerVariable*>(v.get());
  return std::pair<int, int>(clone->lbound_val(), clone->ubound_val());
}

// -----------------------------------------------------------------------------
std::pair<double, double> GetLinBounds(VariablePtr v) {
  LinearVariable* clone = dynamic_cast<LinearVariable*>(v.get());
  return std::pair<double, double>(clone->lbound_val(), clone->ubound_val());
}

} // namespace cyclus
