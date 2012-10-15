#ifndef SYMBOLICFUNCTIONS_H
#define SYMBOLICFUNCTIONS_H

#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

/// a smart pointer to the abstract class so we can pass it around
class Function;
typedef boost::shared_ptr<Function> FunctionPtr;

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
     @param i the intercept
   */
 LinearFunction(double s, double i) : 
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
     @param i the intercept
   */
 ExponentialFunction(double c, double e, double i) : 
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
   a container for an x,y coordinate
 */
class Point
{
 public:
  /**
     constructor for a point
  */
 Point(double x_, double y_) :x(x_), y(y_) {};
  /// x coordinate
  double x;
  /// y coordinate
  double y;
};

/**
   piecewise function
   f(x) for all x in [lhs,rhs]
   0 otherwise
 */
class PiecewiseFunction : public Function 
{
 public:
  /**
     constructor for a piecewise function
     @param function the function
     @param point the initial point for the function
     @param rhs the rhs bound
   */
 PiecewiseFunction(const FunctionPtr& function, const Point& point, double rhs) :
  function_(function), init_point_(point), rhs_(rhs) {};

  /**
     constructor for a piecewise function, rhs defaults to inf
     @param function the function
     @param point the initial point for the function
   */
  PiecewiseFunction(const FunctionPtr& function, const Point& point);

  /**
     constructor for a piecewise function, rhs defaults to inf,
     initial point defaults to (0,0)
     @param function the function
   */
  PiecewiseFunction(const FunctionPtr& function);
  
  /// evaluation for an double argument
  virtual double value(double x);
  
  /// print a string of the function
  virtual std::string print();

  /// the lhs boundary
  double lhs();
  
  /// the rhs boundary
  double rhs();

  /// the initial point
  Point startingPoint();
  
 private:
  /// the constituent function
  FunctionPtr function_;

  /// the initial point
  Point init_point_;
  
  /// the rhs cutoff
  double rhs_;
};

/**
   a class that contains a series of piecewise functions
 */
class PiecewiseFunctionSeries : public Function 
{
 public:
  /**
     constructor
   */
  PiecewiseFunctionSeries();

  /**
     append a function to the list of functions comprising this series
     @param function the function to append. default behavior is to 
     throw an error if the location is before the lhs of the last 
     appended function.
   */
  void appendFunction(const boost::shared_ptr<PiecewiseFunction>& function);

  /// evaluation for an double argument
  virtual double value(double x);
  
  /// print a string of the function
  virtual std::string print();
  
 private:
  std::list< boost::shared_ptr<PiecewiseFunction> > functions_;
};

#endif
