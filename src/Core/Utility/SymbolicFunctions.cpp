#include "SymbolicFunction.h"

#include <string>
#include <sstream>

using namespace std;


// -------------------------------------------------------------------
virtual double LinearFunction::value(int x) { 
  return slope_ * x + intercept_; 
}

// -------------------------------------------------------------------
virtual double LinearFunction::value(double x) { 
  return slope_ * x + intercept_; 
}

// -------------------------------------------------------------------
virtual double ExponentialFunction::value(int x) { 
 return constant_ * exp(exponent_ * x) + intercept_;
}

// -------------------------------------------------------------------
virtual double ExponentialFunction::value(double x) { 
  return constant_ * exp(exponent_ * x) + intercept_; 
}
