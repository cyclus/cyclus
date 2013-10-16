#include "function.h"

#include <utility>
#include <map>
#include <string>
#include <sstream>

#include "variable.h"
#include "error.h"

// -----------------------------------------------------------------------------
cyclus::Function::Function() {
  constituents_ = std::map<cyclus::VariablePtr, double>();
}

// -----------------------------------------------------------------------------
void cyclus::Function::AddVariable(cyclus::VariablePtr v,
                                             double modifier) {
  constituents_.insert(
      std::pair<cyclus::VariablePtr, double>(v, modifier));
}

// -----------------------------------------------------------------------------
double cyclus::Function::GetModifier(cyclus::VariablePtr v) {
  return constituents_[v];
}

// -----------------------------------------------------------------------------
std::map<cyclus::VariablePtr, double>::const_iterator 
cyclus::Function::begin() {
  return constituents_.begin();
}

// -----------------------------------------------------------------------------
std::map<cyclus::VariablePtr, double>::const_iterator 
cyclus::Function::end() {
  return constituents_.end();
}

// -----------------------------------------------------------------------------
int cyclus::Function::NumVars() { 
  return constituents_.size(); 
}

// -----------------------------------------------------------------------------
std::string cyclus::Function::Print() { 
  std::stringstream ss;
  int count = 0;
  std::map<cyclus::VariablePtr, double>::iterator it;
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
cyclus::Constraint::Constraint(
    cyclus::Constraint::EqualityRelation eq_r, 
    double rhs)
    : eq_relation_(eq_r), 
      rhs_(rhs) 
{ };

// -----------------------------------------------------------------------------
cyclus::Constraint::EqualityRelation 
cyclus::Constraint::eq_relation() { 
  return eq_relation_; 
}

// -----------------------------------------------------------------------------
double cyclus::Constraint::rhs() { 
  return rhs_; 
}

// -----------------------------------------------------------------------------
std::string cyclus::Constraint::Print() {
  std::stringstream ss;
  ss << "s.t. " << cyclus::Function::Print() 
     << " " << EqRToStr() << " " << rhs_;
  return ss.str();
}

// -----------------------------------------------------------------------------
std::string cyclus::Constraint::EqRToStr() {
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
cyclus::ObjectiveFunction::ObjectiveFunction(
    cyclus::ObjectiveFunction::Direction dir) : dir_(dir) 
{ }

// -----------------------------------------------------------------------------
cyclus::ObjectiveFunction::Direction 
cyclus::ObjectiveFunction::dir() {
  return dir_;
}

// -----------------------------------------------------------------------------
std::string cyclus::ObjectiveFunction::Print() {
  std::stringstream ss;
  ss << DirToStr() << " " << Function::Print();
  return ss.str();
}

// -----------------------------------------------------------------------------
std::string cyclus::ObjectiveFunction::DirToStr() {
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
