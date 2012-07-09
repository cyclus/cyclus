#include "SymbolicFunctionFactories.h"

#include "SymbolicFunctions.h"
#include "CycException.h"

#include <map>
#include <string>
#include <sstream>

using namespace std;

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

// -------------------------------------------------------------------
std::map<std::string,BasicFunctionFactory::FunctionType> 
BasicFunctionFactory::enum_names_ = 
  map<string,BasicFunctionFactory::FunctionType>();

// -------------------------------------------------------------------
BasicFunctionFactory::BasicFunctionFactory() {
  if (enum_names_.empty()) {
    enum_names_["lin"]=LIN;
    enum_names_["exp"]=EXP;
  }
} 

// -------------------------------------------------------------------
FunctionPtr BasicFunctionFactory::getFunctionPtr(std::string type, 
                                                 std::string params) {
  switch(enum_names_[type]) {
  case LIN:
    {
    LinFunctionFactory lff;
    return lff.getFunctionPtr(params);
    }
    break;
  case EXP:
    {
    ExpFunctionFactory eff;
    return eff.getFunctionPtr(params);
    }
    break;
  default:
    stringstream err("");
    err << type << " is not a registered function type" 
        << " of the basic function factory.";
    throw CycException(err.str());
    break;
  }
}
