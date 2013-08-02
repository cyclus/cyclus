#ifndef CYCLOPTS_VARIABLE_H_
#define CYCLOPTS_VARIABLE_H_

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

namespace cyclus {
namespace cyclopts {
class Variable;

/// a smart pointer to the base class
typedef boost::shared_ptr<Variable> VariablePtr;

/// Variable base class
class Variable {
 public:  
  /// enum for possible bound types
  enum Bound {NEG_INF, INF, FINITE};

  /// enum for possible types
  enum VarType {INT, LINEAR};

  /// constructor, sets id_, lbound_, ubound_
  Variable(Bound l, Bound u, VarType t);

  /// virtual destructor for a base class
  virtual ~Variable() {};

  /// id getter
  int id();

  /// get value
  VarType type();

  /// lbound getter
  Bound lbound();

  /// ubound getter
  Bound ubound();

  /// get name
  std::string name();

  /// set name
  void set_name(std::string name);

  /// get value
  boost::any value();

  /// set value
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
  /// constructor for bounds NEG_INF : INF
  LinearVariable(Bound lb, Bound ub);

  /// constructor for bounds FINITE : INF
  LinearVariable(double lb_val, Bound ub);

  /// constructor for NEG_INF : FINITE
  LinearVariable(Bound lb, double ub_val);

  /// constructor for FINITE : FINITE
  LinearVariable(double lb_val, double ub_val);

  /// lBound value getter
  double lbound_val();

  /// uBound value getter
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
  /// constructor for bounds NEG_INF : INF
  IntegerVariable(Bound lb, Bound ub);

  /// constructor for bounds FINITE : INF
  IntegerVariable(int lb_val, Bound ub);  

  /// constructor for NEG_INF : FINITE
  IntegerVariable(Bound lb, int ub_val);

  /// constructor for FINITE : FINITE
  IntegerVariable(int lb_val, int ub_val);    

  /// lBound value getter
  int lbound_val();

  /// uBound value getter
  int ubound_val();

 private:
  /// lower Bound value
  int lbound_val_;

  /// upper Bound value
  int ubound_val_;
};
} // namespace cyclopts
} // namespace cyclus

#endif
