#include "function.h"

#include <utility>
#include <map>
#include <string>
#include <sstream>

#include "variable.h"
#include "error.h"

namespace cyclus {

// -----------------------------------------------------------------------------
Function::Function() { }

// -----------------------------------------------------------------------------
void Function::AddVariable(Variable::Ptr v, double modifier) {
  constituents_.insert(std::pair<Variable::Ptr, double>(v, modifier));
}

// -----------------------------------------------------------------------------
double Function::GetModifier(Variable::Ptr v) {
  return constituents_[v];
}

// -----------------------------------------------------------------------------
std::map<Variable::Ptr, double>::const_iterator 
Function::begin() {
  return constituents_.begin();
}

// -----------------------------------------------------------------------------
std::map<Variable::Ptr, double>::const_iterator 
Function::end() {
  return constituents_.end();
}

// -----------------------------------------------------------------------------
int Function::NumVars() { 
  return constituents_.size(); 
}

// -----------------------------------------------------------------------------
std::string Function::Print() { 
  std::stringstream ss;
  int count = 0;
  std::map<Variable::Ptr, double>::iterator it;
  for (it = constituents_.begin(); it != constituents_.end(); it++) {
    ss << it->second << it->first->name();
    if (count < constituents_.size()-1) {
      ss << " + ";
    }
    count++;
  }
  return ss.str();
}

// -----------------------------------------------------------------------------
Constraint::Constraint(Constraint::EqualityRelation eq_r, double rhs)
    : eq_relation_(eq_r), 
      rhs_(rhs) 
{ };

// -----------------------------------------------------------------------------
Constraint::EqualityRelation Constraint::eq_relation() { 
  return eq_relation_; 
}

// -----------------------------------------------------------------------------
double Constraint::rhs() { 
  return rhs_; 
}

// -----------------------------------------------------------------------------
std::string Constraint::Print() {
  std::stringstream ss;
  ss << "s.t. " << Function::Print() 
     << " " << EqRToStr() << " " << rhs_;
  return ss.str();
}

// -----------------------------------------------------------------------------
std::string Constraint::EqRToStr() {
  switch(eq_relation_) {
    case EQ:
      return "=";
      break;
    case GT:
      return ">";
      break;
    case GTEQ:
      return ">=";
      break;
    case LT:
      return "<";
      break;
    case LTEQ:
      return "<=";
      break;
    default:
      throw Error("Enumeration value not recognized by Constraint.");
  }
}

// -----------------------------------------------------------------------------
ObjectiveFunction::ObjectiveFunction(ObjectiveFunction::Direction dir)
    : dir_(dir) 
{ }

// -----------------------------------------------------------------------------
ObjectiveFunction::Direction ObjectiveFunction::dir() {
  return dir_;
}

// -----------------------------------------------------------------------------
std::string ObjectiveFunction::Print() {
  std::stringstream ss;
  ss << DirToStr() << " " << Function::Print();
  return ss.str();
}

// -----------------------------------------------------------------------------
std::string ObjectiveFunction::DirToStr() {
  switch(dir_) {
    case MIN:
      return "min";
      break;
    case MAX:
      return "max";
      break;
    default:
      throw Error("Enumeration value not recognized by ObjectiveFunction.");
  }
}

} // namespace cyclus
