#include "variable.h"

#include <string>
#include <sstream>

using namespace std;
using namespace boost;
using namespace cyclopts;

// -----------------------------------------------------------------------------
int Variable::next_id_ = 0;

// -----------------------------------------------------------------------------
Variable::Variable(Bound l, Bound u, VarType t)
    : id_(next_id_++),
      lbound_(l),
      ubound_(u),
      type_(t) {
  stringstream ss;
  ss << "var_" << id_;
  name_ = ss.str();
}

// -----------------------------------------------------------------------------
int Variable::id() {return id_;}

// -----------------------------------------------------------------------------
Variable::VarType Variable::type() {return type_;}

// -----------------------------------------------------------------------------
Variable::Bound Variable::lbound() {return lbound_;}

// -----------------------------------------------------------------------------
Variable::Bound Variable::ubound() {return ubound_;}

// -----------------------------------------------------------------------------
std::string Variable::name() {return name_;}

// -----------------------------------------------------------------------------
void Variable::set_name(std::string name) {name_ = name;}

// -----------------------------------------------------------------------------
void Variable::set_value(any v) {value_ = v;}

// -----------------------------------------------------------------------------
any Variable::value() {return value_;}

// -----------------------------------------------------------------------------
LinearVariable::LinearVariable(Bound lb, Bound ub)
    : Variable(lb,ub,LINEAR),
      lbound_val_(0.0),
      ubound_val_(0.0) {};

// -----------------------------------------------------------------------------
LinearVariable::LinearVariable(double lb_val, Bound ub)
    : Variable(FINITE,ub,LINEAR),
      lbound_val_(lb_val),
      ubound_val_(0.0) {}; 
  
// -----------------------------------------------------------------------------
LinearVariable::LinearVariable(Bound lb, double ub_val)
    : Variable(lb,FINITE,LINEAR),
      lbound_val_(0.0),
      ubound_val_(ub_val) {}; 

// -----------------------------------------------------------------------------
LinearVariable::LinearVariable(double lb_val, double ub_val)
    : Variable(FINITE,FINITE,LINEAR),
      lbound_val_(lb_val),
      ubound_val_(ub_val) {}; 

// -----------------------------------------------------------------------------
double LinearVariable::lbound_val() {return lbound_val_;}

// -----------------------------------------------------------------------------
double LinearVariable::ubound_val() {return ubound_val_;}

// -----------------------------------------------------------------------------
IntegerVariable::IntegerVariable(Bound lb, Bound ub)
    : Variable(lb,ub,INT),
      lbound_val_(0),
      ubound_val_(0) {};

// -----------------------------------------------------------------------------
IntegerVariable::IntegerVariable(int lb_val, Bound ub)
    : Variable(FINITE,ub,INT),
      lbound_val_(lb_val),
      ubound_val_(0) {}; 
  
// -----------------------------------------------------------------------------
IntegerVariable::IntegerVariable(Bound lb, int ub_val)
    : Variable(lb,FINITE,INT),
      lbound_val_(0),
      ubound_val_(ub_val) {}; 

// -----------------------------------------------------------------------------
IntegerVariable::IntegerVariable(int lb_val, int ub_val)
    : Variable(FINITE,FINITE,INT),
      lbound_val_(lb_val),
      ubound_val_(ub_val) {}; 

// -----------------------------------------------------------------------------
int IntegerVariable::lbound_val() {return lbound_val_;}

// -----------------------------------------------------------------------------
int IntegerVariable::ubound_val() {return ubound_val_;}
