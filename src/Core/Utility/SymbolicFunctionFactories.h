#ifndef SYMBOLICFUNCTIONFACTORIES_H
#define SYMBOLICFUNCTIONFACTORIES_H

#include "SymbolicFunctions.h"

#include <string>
#include <map>

/**
   An abstract factory for pointers to symbolic functions
 */
class SymbFunctionFactory {
 public:
  /// virtual destructor for an abstract base class
  virtual ~SymbFunctionFactory() {};

  /**
     a virtual function that must be defined by derived classes
     @param params a string of required parameters for the function
     @return a FunctionPtr to the constructed function
   */
  virtual FunctionPtr getFunctionPtr(std::string params) = 0;
};

/**
   a concrete factory for linear functions
 */
class LinFunctionFactory : public SymbFunctionFactory {
 public:
  /**
     return a function pointer to a linear function
     @param params the parameters for the linear function in order:
     slope, intercept
     @return the linear function
   */
  virtual FunctionPtr getFunctionPtr(std::string params);
};

/**
   a concrete factory for exponential functions
 */
class ExpFunctionFactory : public SymbFunctionFactory {
 public:
  /**
     return a function pointer to a exponential function
     @param params the parameters for the exponential function in
     order: constant, exponent, intercept
     @return the exponential function
   */
  virtual FunctionPtr getFunctionPtr(std::string params);
};

/**
   a concrete factory that can provide access to  basic symbolic 
   functions
 */
class BasicFunctionFactory {
 public:
  /// the type of functions this factory can provide
  enum FunctionType {LIN,EXP};
  
  /**
     constructor sets up the enum names map
   */
  BasicFunctionFactory();

  /**
     return a function pointer to a registered function type
     @param type the function type
     @param params the function parameters
     @return the function
   */
  FunctionPtr getFunctionPtr(std::string type, std::string params);
    
 private:
  /// a map between enums and names
  static std::map<std::string,BasicFunctionFactory::FunctionType> 
    enum_names_;
};


#endif
