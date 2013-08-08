#include "logger.h"
#include "error.h"

#include "symbolic_function_factories.h"

//using namespace boost;

namespace cyclus {

// -------------------------------------------------------------------
FunctionPtr LinFunctionFactory::GetFunctionPtr(std::string params) {
  std::stringstream ss(params);
  double slope, intercept;
  ss >> slope >> intercept;

  LOG(LEV_DEBUG2, "Funct") <<
                           "Linear function created in the form y = m*x + b, with";
  LOG(LEV_DEBUG2, "Funct") << "  * m: " << slope;
  LOG(LEV_DEBUG2, "Funct") << "  * b: " << intercept;

  return FunctionPtr(new LinearFunction(slope, intercept));
}

// -------------------------------------------------------------------
FunctionPtr ExpFunctionFactory::GetFunctionPtr(std::string params) {
  std::stringstream ss(params);
  double constant, exponent, intercept;
  ss >> constant >> exponent >> intercept;

  LOG(LEV_DEBUG2, "Funct") <<
                           "Exponential function created in the form y = a*exp(b*x) + c, with";
  LOG(LEV_DEBUG2, "Funct") << "  * a: " << constant;
  LOG(LEV_DEBUG2, "Funct") << "  * b: " << exponent;
  LOG(LEV_DEBUG2, "Funct") << "  * c: " << intercept;

  return FunctionPtr(new ExponentialFunction(constant, exponent,
                                             intercept));
}

// -------------------------------------------------------------------
PiecewiseFunctionFactory::PiecewiseFunctionFactory() {
  function_ = boost::shared_ptr<PiecewiseFunction>(new PiecewiseFunction());
}

// -------------------------------------------------------------------
FunctionPtr PiecewiseFunctionFactory::GetFunctionPtr(std::string params) {
  if (!params.empty()) {
    throw Error("Piecewise Functions cannot be created with a list of parameters");
  }

  LOG(LEV_DEBUG2, "Funct") << "A piecewise function has been created: "
                           << function_->Print();

  return function_;
}

// -------------------------------------------------------------------
void PiecewiseFunctionFactory::AddFunction(FunctionPtr function,
                                           double starting_coord, bool continuous) {
  if (!function_->functions_.empty()) {
    const PiecewiseFunction::PiecewiseFunctionInfo& last =
      function_->functions_.back();
    if (starting_coord <= last.xoffset) {
      throw Error("Cannot append a function before the last registered function");
    }
  }

  double yoffset = 0;
  if (continuous) {
    yoffset = function_->value(starting_coord) - function->value(0);
  }

  function_->functions_.push_back(PiecewiseFunction::PiecewiseFunctionInfo(
                                    function, starting_coord, yoffset));
}

// -------------------------------------------------------------------
std::map<std::string, BasicFunctionFactory::FunctionType>
BasicFunctionFactory::enum_names_ =
  std::map<std::string, BasicFunctionFactory::FunctionType>();

// -------------------------------------------------------------------
BasicFunctionFactory::BasicFunctionFactory() {
  if (enum_names_.empty()) {
    enum_names_["lin"] = LIN;
    enum_names_["exp"] = EXP;
  }
}

// -------------------------------------------------------------------
FunctionPtr BasicFunctionFactory::GetFunctionPtr(std::string type,
                                                 std::string params) {
  switch (enum_names_[type]) {
    case LIN: {
      LinFunctionFactory lff;
      return lff.GetFunctionPtr(params);
    }
    break;
    case EXP: {
      ExpFunctionFactory eff;
      return eff.GetFunctionPtr(params);
    }
    break;
    default:
      std::stringstream err("");
      err << type << " is not a registered function type"
          << " of the basic function factory.";
      throw Error(err.str());
      break;
  }
}
} // namespace cyclus
