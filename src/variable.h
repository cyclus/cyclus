#ifndef CYCLUS_SRC_VARIABLE_H_
#define CYCLUS_SRC_VARIABLE_H_

#include <string>
#include <utility>

#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

namespace cyclus {

class Variable;

/// Variable base class
class Variable {
 public:
  /// a smart pointer to the base class
  typedef boost::shared_ptr<Variable> Ptr;

  /// enum for possible bound types
  enum Bound {NEG_INF, INF, FINITE};

  /// enum for possible types
  enum VarType {INT, LINEAR};

  /// constructor
  /// @param l the lower bound
  /// @param u the upper bound
  /// @param t the variable type
  Variable(Bound l, Bound u, VarType t);

  /// virtual destructor for a base class
  virtual ~Variable() {}

  /// @return the variable's id
  int id();

  /// @return variable type
  VarType type();

  /// @return lower bound type
  Bound lbound();

  /// @return upper bound type
  Bound ubound();

  /// @return name
  std::string name();

  /// set name
  /// @param name the name
  void set_name(std::string name);

  /// @return value
  boost::any value();

  /// set value
  /// @param v the value
  void set_value(boost::any v);

 private:
  /// global id incrementor
  static int next_id_;

  /// id
  int id_;

  /// type
  VarType type_;

  /// lower bound
  Bound lbound_;

  /// upper bound
  Bound ubound_;

  /// name
  std::string name_;

  /// value
  boost::any value_;
};

/// derived class for linear variables
class LinearVariable : public Variable {
 public:
  /// constructor
  /// @param lb the non-finite lower bound type
  /// @param ub the non-finite upper bound type
  LinearVariable(Bound lb, Bound ub);

  /// constructor
  /// @param lb_val the value of the finite lower bound
  /// @param ub the non-finite upper bound type
  LinearVariable(double lb_val, Bound ub);

  /// constructor
  /// @param lb the non-finite lower bound type
  /// @param ub_val the value of the finite upper bound
  LinearVariable(Bound lb, double ub_val);

  /// constructor
  /// @param lb_val the value of the non-finite lower bound
  /// @param ub_val the value of the non-finite upper bound
  LinearVariable(double lb_val, double ub_val);

  /// @return lower bound value
  double lbound_val();

  /// @return upper bound value
  double ubound_val();

 private:
  /// lower Bound value
  double lbound_val_;

  /// upper Bound value
  double ubound_val_;
};

/// derived class for integer variables
class IntegerVariable : public Variable {
 public:
  /// constructor
  /// @param lb the non-finite lower bound type
  /// @param ub the non-finite upper bound type
  IntegerVariable(Bound lb, Bound ub);

  /// constructor
  /// @param lb_val the value of the finite lower bound
  /// @param ub the non-finite upper bound type
  IntegerVariable(int lb_val, Bound ub);

  /// constructor
  /// @param lb the non-finite lower bound type
  /// @param ub_val the value of the finite upper bound
  IntegerVariable(Bound lb, int ub_val);

  /// constructor
  /// @param lb_val the value of the non-finite lower bound
  /// @param ub_val the value of the non-finite upper bound
  IntegerVariable(int lb_val, int ub_val);

  /// @return lower bound value
  int lbound_val();

  /// @return upper bound value
  int ubound_val();

 private:
  /// lower Bound value
  int lbound_val_;

  /// upper Bound value
  int ubound_val_;
};

/// @return a pair of integers representing the lower and upper bounds of an
/// integer variable
std::pair<int, int> GetIntBounds(Variable::Ptr v);

/// @return a pair of doubles representing the lower and upper bounds of a
/// linear variable
std::pair<double, double> GetLinBounds(Variable::Ptr v);

}  // namespace cyclus

#endif  // CYCLUS_SRC_VARIABLE_H_
