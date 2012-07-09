#include "SymbolicFunctions.h"

#include <string>
#include <sstream>
#include <math.h>

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

// -------------------------------------------------------------------
FunctionPtr LinFunctionFactory::getFunctionPtr(std::string params) { 
  stringstream ss(params);
  double slope, intercept;
  ss >> slope >> intercept;
  return FunctionPtr(new LinearFunction(slope,intercept));
}

// -------------------------------------------------------------------
FunctionPtr ExpFunctionFactory::getFunctionPtr(std::string params) { 
  stringstream ss(params);
  double constant, exponent, intercept;
  ss >> constant >> exponent >> intercept;
  return FunctionPtr(new ExponentialFunction(constant,exponent,
                                             intercept));
}
