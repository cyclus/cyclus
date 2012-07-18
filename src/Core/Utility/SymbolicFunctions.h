#ifndef SYMBOLICFUNCTIONS_H
#define SYMBOLICFUNCTIONS_H

#include <string>
#include <boost/shared_ptr.hpp>

/// a smart pointer to the abstract class so we can pass it around
class Function;
typedef boost::shared_ptr<Function> FunctionPtr;

/// abstract base class for symbolic functions
class Function {
 public:
  /// virtual destructor for an abstract base class
  virtual ~Function() {}; 
  /// base class must define how to calculate demand (int argument)
  virtual double value(int x) = 0; 
  /// base class must define how to calculate demand (dbl argument)
  virtual double value(double x) = 0; 
  /// every function must print itself
  virtual std::string print() = 0;
  /* /// overload function operator */
  /* double operator()(int x) {return value(x);} */
  /* /// overload function operator */
  /* double operator()(double x) {return value(x);} */
};

/**
   linear functions
   f(x) = slope_ * x + intercept_
 */
class LinearFunction : public Function {
 public:
  /**
     constructor for a linear function
     @param s the slope
     @param i the intercept
   */
 LinearFunction(double s, double i) : 
  slope_(s), intercept_(i) {};
  
  /// evaluation for an integer argument
  virtual double value(int x);
  
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
class ExponentialFunction : public Function {
 public:
  /**
     constructor for an exponential function
     @param c the leading constant
     @param e the exponent multiplier
     @param i the intercept
   */
 ExponentialFunction(double c, double e, double i) : 
  constant_(c), exponent_(e), intercept_(i) {};

  /// evaluation for an integer argument
  virtual double value(int x);

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

#endif
