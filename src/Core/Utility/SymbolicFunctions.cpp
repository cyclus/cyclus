#include "SymbolicFunctions.h"

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
double ExponentialFunction::value(int x) { 
 return constant_ * exp(exponent_ * x) + intercept_;
}

// -------------------------------------------------------------------
double ExponentialFunction::value(double x) { 
  return constant_ * exp(exponent_ * x) + intercept_; 
}
