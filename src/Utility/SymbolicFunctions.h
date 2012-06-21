#ifndef SYMBOLICFUNCTIONS_H
#define SYMBOLICFUNCTIONS_H

#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

#include <boost/shared_ptr.hpp>

/// a smart pointer to the abstract class so we can pass it around
class Function;
typedef boost::shared_ptr<Function> FunctionPtr;

/// abstract base class
class Function {
 public:
  /// virtual destructor for an abstract base class
  virtual ~Function() {}; 
  /// base class must define how to calculate demand
  virtual double value(int x) = 0; 
  /// base class must define how to calculate demand
  virtual double value(double x) = 0; 
  /// overload function operator
  double operator()(int x) {return value(x);}
  /// overload function operator
  double operator()(double x) {return value(x);}
};

/**
   linear functions
   f(x) = slope_ * x + intercept_
 */
class LinearFunction : public Function {
 public:
  /// constructor
 LinearFunction(double s, double i) : 
  slope_(s), intercept_(i) {};
  
  /// evaluations
  virtual double value(int x) { return slope_ * x + intercept_; }
  virtual double value(double x) { return slope_ * x + intercept_; }

 private:
  double slope_;
  double intercept_;
};

/**
   exponential functions
   f(x) = constant_ * exp ( exponent_ * x ) + intercept_
 */
class ExponentialFunction : public Function {
 public:
  /// constructor
 ExponentialFunction(double c, double e, double i) : 
  constant_(c), exponent_(e), intercept_(i) {};

  /// evaluations
  virtual double value(int x) { return constant_ * exp(exponent_ * x) + intercept_; }
  virtual double value(double x) { return constant_ * exp(exponent_ * x) + intercept_; }

 private:
  double constant_;
  double exponent_;
  double intercept_;
};

#endif
