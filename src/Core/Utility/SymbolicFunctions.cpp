#include "SymbolicFunctions.h"

#include <math.h>
#include <string>
#include <sstream>

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

// // -------------------------------------------------------------------
// double PiecewiseFunction::value(int x) 
// { 
// }

// // -------------------------------------------------------------------
// double PiecewiseFunction::value(double x) 
// { 
// }

// // -------------------------------------------------------------------
// std::string PiecewiseFunction::print() { 
//   stringstream ss("");
//   ss << function_->print() 
//      << " valid on [" << lhs_ << "," << rhs_ << "]";
//   return ss.str();
// }
