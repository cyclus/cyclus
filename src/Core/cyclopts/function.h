#ifndef CYCLOPTS_FUNCTION_H_
#define CYCLOPTS_FUNCTION_H_

#include <map>
#include <utility>
#include <string>

#include <boost/shared_ptr.hpp>

#include "variable.h"

namespace cyclopts {
class Constraint;
typedef boost::shared_ptr<Constraint> ConstraintPtr;
class ObjectiveFunction;
typedef boost::shared_ptr<ObjectiveFunction> ObjFuncPtr;
}

#include "solver_interface.h"

namespace cyclopts {
/// function base class
class Function {
 public:
  /// constructor
  Function();

  /// virtual destructor for base class
  virtual ~Function() {};

  /// get a modifier
  double GetModifier(VariablePtr v);

  /// get the beginning iterator to constituents_
  std::map<VariablePtr,double>::iterator begin();

  /// get the ending iterator to constituents_
  std::map<VariablePtr,double>::iterator end();

  /// get number of constituents
  int NumConstituents();

  /// print the function
  virtual std::string Print();

 private:
  /// a container of all variables and their corresponding constant
  std::map<VariablePtr,double> constituents_;    

  /// add a constituent
  void AddConstituent(VariablePtr v, double modifer);

  /// the solver interface builds functions knowing what variables exist
  friend class SolverInterface;
};

/// derived class for constraints
class Constraint : public Function {
 public:
  /// the equality relation
  enum EqualityRelation {EQ,GT,GTEQ,LT,LTEQ};
  
  /// constructor
  Constraint(EqualityRelation eq_r, double rhs);

  /// get the equality relation
  EqualityRelation eq_relation();

  /// get the rhs
  double rhs();

  /// print the constraint
  virtual std::string Print();

 private:
  /// the type of equality relation
  EqualityRelation eq_relation_;

  /// the rhs value
  double rhs_;

  /// turn eq_relation_ into a string
  std::string EqRToStr();
};
  
/// derived class for objective functions
class ObjectiveFunction : public Function {
 public: 
  /// the possible direction
  enum Direction {MIN,MAX};

  /// constructor
  explicit ObjectiveFunction(Direction dir);

  /// get the Direction
  Direction dir();

  /// print the objective function
  virtual std::string Print();

 private:
  /// the Direction
  Direction dir_;

  /// turn dir_ into a string
  std::string DirToStr();
};
}

#endif
