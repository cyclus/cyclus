#ifndef CYCLUS_CYCLOPTS_FUNCTION_H_
#define CYCLUS_CYCLOPTS_FUNCTION_H_

#include <map>
#include <utility>
#include <string>

#include <boost/shared_ptr.hpp>

#include "variable.h"

namespace cyclus {
namespace cyclopts {
class Constraint;
typedef boost::shared_ptr<Constraint> ConstraintPtr;
class ObjectiveFunction;
typedef boost::shared_ptr<ObjectiveFunction> ObjFuncPtr;
} // namespace cyclopts
} // namespace cyclus


#include "solver_interface.h"

namespace cyclus {
namespace cyclopts {
/// function base class
class Function {
 public:
  /// constructor
  Function();

  /// virtual destructor for base class
  virtual ~Function() {};

  /// get a modifier
  /// @param v the variable being modified
  double GetModifier(cyclus::cyclopts::VariablePtr v);

  /// @return the beginning iterator to the function variable
  std::map<cyclus::cyclopts::VariablePtr, double>::iterator begin();

  /// @return the ending iterator to the function variable
  std::map<cyclus::cyclopts::VariablePtr, double>::iterator end();

  /// add a variable to the constraitn
  /// @param v a pointer to the variable to add
  /// @param modifier the modifier for that variable in the function
  void AddVariable(cyclus::cyclopts::VariablePtr v, double modifer);

  /// @return number of variables in the function
  int NumVars();

  /// print the function
  virtual std::string Print();

 private:
  /// a container of all variables and their corresponding constant
  std::map<cyclus::cyclopts::VariablePtr, double> constituents_;    
};

/// derived class for constraints
class Constraint : public Function {
 public:
  /// the equality relation
  enum EqualityRelation {EQ, GT, GTEQ, LT, LTEQ};
  
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
  enum Direction {MIN, MAX};

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
} // namespace cyclopts
} // namespace cyclus

#endif
