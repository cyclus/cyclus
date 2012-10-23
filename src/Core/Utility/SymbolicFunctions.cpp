#include "SymbolicFunctions.h"

#include <math.h>
#include <string>
#include <sstream>

using namespace std;

// -------------------------------------------------------------------
double LinearFunction::value(int x) { 
  return slope_ * x + intercept_; 
}

// -------------------------------------------------------------------
double LinearFunction::value(double x) { 
  return slope_ * x + intercept_; 
}

// -------------------------------------------------------------------
std::string LinearFunction::print() { 
  stringstream ss("");
  ss << "y = " << slope_ << " * x + " << intercept_;
  return ss.str();
}

// -------------------------------------------------------------------
double ExponentialFunction::value(int x) { 
 return constant_ * exp(exponent_ * x) + intercept_;
}

// -------------------------------------------------------------------
double ExponentialFunction::value(double x) { 
  return constant_ * exp(exponent_ * x) + intercept_; 
}

// -------------------------------------------------------------------
std::string ExponentialFunction::print() { 
  stringstream ss("");
  ss << "y = " << constant_ 
     <<" * exp(" << exponent_ << " * x) + " << intercept_;
  return ss.str();
}
