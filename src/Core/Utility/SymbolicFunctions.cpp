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
PiecewiseFunction::PiecewiseFunction(FunctionPtr function, Point point) :
  function_(function), 
  init_point_(point), 
  rhs_(numeric_limits<double>::max()) 
{}

// -------------------------------------------------------------------
PiecewiseFunction::PiecewiseFunction(FunctionPtr function) :
  function_(function), 
  init_point_(Point(0.0,0.0)), 
  rhs_(numeric_limits<double>::max()) 
{}

// -------------------------------------------------------------------
double PiecewiseFunction::value(double x) 
{ 
  double value;
  if (x < lhs() || x > rhs()) value = 0.0;
  else value = function_->value(x - init_point_.x) + init_point_.y;
  return value;
}

// -------------------------------------------------------------------
std::string PiecewiseFunction::print() 
{ 
  stringstream ss("");
  ss << function_->print() 
     << " valid on [" << init_point_.x << "," << rhs_ << "]"
     << " starting at coordinate (" 
     << init_point_.x << "," << init_point_.y << ")";
  return ss.str();
}

// -------------------------------------------------------------------
double PiecewiseFunction::lhs() 
{
  return init_point_.x;
}

// -------------------------------------------------------------------
double PiecewiseFunction::rhs() 
{
  return rhs_;
}
