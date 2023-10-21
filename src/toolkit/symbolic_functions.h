#ifndef CYCLUS_SRC_TOOLKIT_SYMBOLIC_FUNCTIONS_H_
#define CYCLUS_SRC_TOOLKIT_SYMBOLIC_FUNCTIONS_H_

#include <list>
#include <string>

#include <boost/shared_ptr.hpp>

namespace cyclus {
namespace toolkit {

// Forward declarations
class LinearFunction;
class ExponentialFunction;
class PiecewiseFunction;

/// Abstract base class for symbolic functions
class SymFunction {
 public:
  typedef boost::shared_ptr<SymFunction> Ptr;

  /// Virtual destructor for an abstract base class
  virtual ~SymFunction() {}

  /// Base class must define how to calculate demand (dbl argument)
  virtual double value(double x) = 0;

  /// Every function must print itself
  virtual std::string Print() = 0;
};

/// Linear functions
/// f(x) = slope_ * x + intercept_
class LinearFunction : public SymFunction {
 public:
  /// Constructor for a linear function
  /// @param s the slope
  /// @param i the intercept, with the default being 0
  LinearFunction(double s, double i = 0.0) : slope_(s), intercept_(i) {}

  /// Evaluation for an double argument
  virtual double value(double x);

  /// Print a string of the function
  virtual std::string Print();

 private:
  /// The slope
  double slope_;

  /// The intercept
  double intercept_;
};

/// Exponential functions
/// f(x) = constant_ * exp ( exponent_ * x ) + intercept_
class ExponentialFunction : public SymFunction {
 public:
  /// Constructor for an exponential function
  /// @param c the leading constant
  /// @param e the exponent multiplier
  /// @param i the intercept, with the default being 0
  ExponentialFunction(double c, double e, double i = 0.0)
      : constant_(c),
        exponent_(e),
        intercept_(i) {}

  /// Evaluation for a double argument
  virtual double value(double x);

  /// Print a string of the function
  virtual std::string Print();

 private:
  /// The constant factor
  double constant_;

  /// The exponent multiplier
  double exponent_;

  /// The intercept
  double intercept_;
};

/// Piecewise function
/// f(x) for all x in [lhs,rhs]
/// 0 otherwise
class PiecewiseFunction : public SymFunction {
  struct PiecewiseFunctionInfo {
    PiecewiseFunctionInfo(SymFunction::Ptr function_, double xoff_ = 0,
                          double yoff_ = 0)
        : function(function_),
          xoffset(xoff_),
          yoffset(yoff_) {}

    SymFunction::Ptr function;
    double xoffset, yoffset;
  };

 public:
  /// Evaluation for an double argument
  virtual double value(double x);

  /// Print a string of the function
  virtual std::string Print();

 private:
  std::list<PiecewiseFunctionInfo> functions_;

  friend class PiecewiseFunctionFactory;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_SYMBOLIC_FUNCTIONS_H_
