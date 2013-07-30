#ifndef SYMBOLICFUNCTIONS_H
#define SYMBOLICFUNCTIONS_H

#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

namespace cyclus {

// forward declarations
class Function;
class LinearFunction;
class ExponentialFunction;
class PiecewiseFunction;

// typedefs
typedef boost::shared_ptr<Function> FunctionPtr;

// forward includes
#include "SymbolicFunctionFactories.h"

/// abstract base class for symbolic functions
class Function 
{
 public:
  /// virtual destructor for an abstract base class
  virtual ~Function() {}; 

  /// base class must define how to calculate demand (dbl argument)
  virtual double value(double x) = 0; 

  /// every function must print itself
  virtual std::string print() = 0;
};

/**
   linear functions
   f(x) = slope_ * x + intercept_
 */
class LinearFunction : public Function 
{
 public:
  /**
     constructor for a linear function
     @param s the slope
     @param i the intercept, with the default being 0
   */
 LinearFunction(double s, double i = 0.0) : 
  slope_(s), intercept_(i) {};
  
  /// evaluation for an double argument
  virtual double value(double x);
  
  /// print a string of the function
  virtual std::string print();

 private:
  /// the slope
  double slope_;

  /// the intercept
  double intercept_;
};

/**
   exponential functions
   f(x) = constant_ * exp ( exponent_ * x ) + intercept_
 */
class ExponentialFunction : public Function 
{
 public:
  /**
     constructor for an exponential function
     @param c the leading constant
     @param e the exponent multiplier
     @param i the intercept, with the default being 0
   */
 ExponentialFunction(double c, double e, double i = 0.0) : 
  constant_(c), exponent_(e), intercept_(i) {};

  /// evaluation for a double argument
  virtual double value(double x);

  /// print a string of the function
  virtual std::string print();

 private:
  /// the constant factor
  double constant_;

  /// the exponent multiplier
  double exponent_;

  /// the intercept
  double intercept_;
};

/**
   piecewise function
   f(x) for all x in [lhs,rhs]
   0 otherwise
 */
class PiecewiseFunction : public Function 
{
  struct PiecewiseFunctionInfo
  {
  PiecewiseFunctionInfo(FunctionPtr function_, double xoff_ = 0, double yoff_ = 0) :
    function(function_), xoffset(xoff_), yoffset(yoff_) {};
    FunctionPtr function;
    double xoffset, yoffset;
  };
  
 public:
  /// evaluation for an double argument
  virtual double value(double x);
  
  /// print a string of the function
  virtual std::string print();
  
 private:
  std::list<PiecewiseFunctionInfo> functions_;

  friend class PiecewiseFunctionFactory;
};
} // namespace cyclus
#endif
