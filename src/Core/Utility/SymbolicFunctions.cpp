#include "SymbolicFunctions.h"

#include <math.h>
#include <string>
#include <sstream>
#include <limits>

using namespace std;

// -------------------------------------------------------------------
double LinearFunction::value(double x) 
{ 
  return slope_ * x + intercept_; 
}

// -------------------------------------------------------------------
std::string LinearFunction::print() 
{ 
  stringstream ss("");
  ss << "y = " << slope_ << " * x + " << intercept_;
  return ss.str();
}

// -------------------------------------------------------------------
double ExponentialFunction::value(double x) 
{ 
  return constant_ * exp(exponent_ * x) + intercept_; 
}

// -------------------------------------------------------------------
std::string ExponentialFunction::print() 
{ 
  stringstream ss("");
  ss << "y = " << constant_ 
     <<" * exp(" << exponent_ << " * x) + " << intercept_;
  return ss.str();
}

// -------------------------------------------------------------------
PiecewiseFunction::PiecewiseFunction(FunctionPtr function, double lhs) :
function_(function), lhs_(lhs), rhs_(numeric_limits<double>::max()) {}

// -------------------------------------------------------------------
PiecewiseFunction::PiecewiseFunction(FunctionPtr function) :
function_(function), lhs_(0.0), rhs_(numeric_limits<double>::max()) {}

// -------------------------------------------------------------------
double PiecewiseFunction::value(double x) 
{ 
  double value;
  if (x < lhs_ || x > rhs_) value = 0.0;
  else value = function_->value(x);
  return value;
}

// -------------------------------------------------------------------
std::string PiecewiseFunction::print() 
{ 
  stringstream ss("");
  ss << function_->print() 
     << " valid on [" << lhs_ << "," << rhs_ << "]";
  return ss.str();
}

// -------------------------------------------------------------------
double PiecewiseFunction::lhs() 
{
  return lhs_;
}

// -------------------------------------------------------------------
double PiecewiseFunction::rhs() 
{
  return rhs_;
}
