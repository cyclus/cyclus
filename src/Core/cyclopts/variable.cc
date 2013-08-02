#include "variable.h"

#include <string>
#include <sstream>

using namespace std;
using namespace boost;

// -----------------------------------------------------------------------------
int cyclus::cyclopts::Variable::next_id_ = 0;

// -----------------------------------------------------------------------------
cyclus::cyclopts::Variable::Variable(Bound l, Bound u, VarType t)
    : id_(next_id_++),
      lbound_(l),
      ubound_(u),
      type_(t) {
  stringstream ss;
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
void cyclus::cyclopts::Variable::set_value(any v) {
  value_ = v;
}

// -----------------------------------------------------------------------------
any cyclus::cyclopts::Variable::value() {
  return value_;
}

// -----------------------------------------------------------------------------
cyclus::cyclopts::LinearVariable::LinearVariable(Bound lb, Bound ub)
    : Variable(lb,ub,LINEAR),
      lbound_val_(0.0),
      ubound_val_(0.0) 
{ }

// -----------------------------------------------------------------------------
cyclus::cyclopts::LinearVariable::LinearVariable(double lb_val, Bound ub)
    : Variable(FINITE,ub,LINEAR),
      lbound_val_(lb_val),
      ubound_val_(0.0) 
{ }
  
// -----------------------------------------------------------------------------
cyclus::cyclopts::LinearVariable::LinearVariable(Bound lb, double ub_val)
    : Variable(lb,FINITE,LINEAR),
      lbound_val_(0.0),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
cyclus::cyclopts::LinearVariable::LinearVariable(double lb_val, double ub_val)
    : Variable(FINITE,FINITE,LINEAR),
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
    : Variable(lb,ub,INT),
      lbound_val_(0),
      ubound_val_(0) 
{ }

// -----------------------------------------------------------------------------
cyclus::cyclopts::IntegerVariable::IntegerVariable(int lb_val, Bound ub)
    : Variable(FINITE,ub,INT),
      lbound_val_(lb_val),
      ubound_val_(0) 
{ }
  
// -----------------------------------------------------------------------------
cyclus::cyclopts::IntegerVariable::IntegerVariable(Bound lb, int ub_val)
    : Variable(lb,FINITE,INT),
      lbound_val_(0),
      ubound_val_(ub_val) 
{ } 

// -----------------------------------------------------------------------------
cyclus::cyclopts::IntegerVariable::IntegerVariable(int lb_val, int ub_val)
    : Variable(FINITE,FINITE,INT),
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
