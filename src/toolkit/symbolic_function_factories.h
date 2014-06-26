#ifndef CYCLUS_SRC_TOOLKIT_SYMBOLIC_FUNCTION_FACTORIES_H_
#define CYCLUS_SRC_TOOLKIT_SYMBOLIC_FUNCTION_FACTORIES_H_

#include <string>
#include <map>
#include <sstream>

#include "symbolic_functions.h"

namespace cyclus {
namespace toolkit {


/// An abstract factory for pointers to symbolic functions
class SymbFunctionFactory {
 public:
  /// Virtual destructor for an abstract base class
  virtual ~SymbFunctionFactory() {}

  ///  A virtual function that must be defined by derived classes
  ///  @param params a string of required parameters for the function
  ///  @return a FunctionPtr to the constructed function
  virtual SymFunction::Ptr GetFunctionPtr(std::string params) = 0;
};

/// A concrete factory for linear functions
class LinFunctionFactory : public SymbFunctionFactory {
 public:
  /// Return a function pointer to a linear function
  /// @param params the parameters for the linear function in order:
  /// slope, intercept
  /// @return the linear function
  virtual SymFunction::Ptr GetFunctionPtr(std::string params);
};

/// A concrete factory for exponential functions
class ExpFunctionFactory : public SymbFunctionFactory {
 public:
  /// Return a function pointer to a exponential function
  /// @param params the parameters for the exponential function in
  /// order: constant, exponent, intercept
  /// @return the exponential function
  virtual SymFunction::Ptr GetFunctionPtr(std::string params);
};

/// A concrete factory for piecewise functions
class PiecewiseFunctionFactory : public SymbFunctionFactory {
 public:
  /// Constructor
  PiecewiseFunctionFactory();

  /// Return a function pointer to a piecewise function
  /// @param params an empty string by default. if this is not empty,
  /// an error is thrown
  /// @return the piecewise function
  virtual SymFunction::Ptr GetFunctionPtr(std::string params = "");

  /// Add a function to the piecewise function being constructed
  /// @param function the function to append
  /// @param starting_coord the x coordinate to begin this function
  /// @param continuous  if true, the added function and previous
  /// function will be continuous, if false, discontinuous
  void AddFunction(SymFunction::Ptr function, double starting_coord = 0.0,
                   bool continuous = true);

 private:
  /// The piecewise function to construct
  boost::shared_ptr<PiecewiseFunction> function_;
};

/// A concrete factory that can provide access to  basic symbolic
/// functions.
class BasicFunctionFactory {
 public:
  /// The type of functions this factory can provide
  enum FunctionType { LIN, EXP };

  /// Constructor sets up the enum names map
  BasicFunctionFactory();

  /// Return a function pointer to a registered function type
  /// @param type the function type
  /// @param params the function parameters
  /// @return the function
  SymFunction::Ptr GetFunctionPtr(std::string type, std::string params);

 private:
  /// A map between enums and names
  static std::map<std::string, BasicFunctionFactory::FunctionType> enum_names_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_SYMBOLIC_FUNCTION_FACTORIES_H_
