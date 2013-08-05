#include "function.h"

#include <utility>
#include <map>
#include <string>
#include <sstream>

#include "variable.h"

// -----------------------------------------------------------------------------
cyclus::cyclopts::Function::Function() {
  constituents_ = std::map<cyclus::cyclopts::VariablePtr, double>();
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::Function::AddVariable(cyclus::cyclopts::VariablePtr v,
                                             double modifier) {
  constituents_.insert(
      std::pair<cyclus::cyclopts::VariablePtr, double>(v, modifier));
}

// -----------------------------------------------------------------------------
double cyclus::cyclopts::Function::GetModifier(cyclus::cyclopts::VariablePtr v) {
  return constituents_[v];
}

// -----------------------------------------------------------------------------
std::map<cyclus::cyclopts::VariablePtr, double>::const_iterator 
cyclus::cyclopts::Function::begin() {
  return constituents_.begin();
}

// -----------------------------------------------------------------------------
std::map<cyclus::cyclopts::VariablePtr, double>::const_iterator 
cyclus::cyclopts::Function::end() {
  return constituents_.end();
}

// -----------------------------------------------------------------------------
int cyclus::cyclopts::Function::NumVars() { 
  return constituents_.size(); 
}

// -----------------------------------------------------------------------------
std::string cyclus::cyclopts::Function::Print() { 
  std::stringstream ss;
  int count = 0;
  std::map<cyclus::cyclopts::VariablePtr, double>::iterator it;
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
cyclus::cyclopts::Constraint::Constraint(
    cyclus::cyclopts::Constraint::EqualityRelation eq_r, 
    double rhs)
    : eq_relation_(eq_r), 
      rhs_(rhs) 
{ };

// -----------------------------------------------------------------------------
cyclus::cyclopts::Constraint::EqualityRelation 
cyclus::cyclopts::Constraint::eq_relation() { 
  return eq_relation_; 
}

// -----------------------------------------------------------------------------
double cyclus::cyclopts::Constraint::rhs() { 
  return rhs_; 
}

// -----------------------------------------------------------------------------
std::string cyclus::cyclopts::Constraint::Print() {
  std::stringstream ss;
  ss << "s.t. " << cyclus::cyclopts::Function::Print() 
     << " " << EqRToStr() << " " << rhs_;
  return ss.str();
}

// -----------------------------------------------------------------------------
std::string cyclus::cyclopts::Constraint::EqRToStr() {
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
  }
}

// -----------------------------------------------------------------------------
cyclus::cyclopts::ObjectiveFunction::ObjectiveFunction(
    cyclus::cyclopts::ObjectiveFunction::Direction dir) : dir_(dir) 
{ }

// -----------------------------------------------------------------------------
cyclus::cyclopts::ObjectiveFunction::Direction 
cyclus::cyclopts::ObjectiveFunction::dir() {
  return dir_;
}

// -----------------------------------------------------------------------------
std::string cyclus::cyclopts::ObjectiveFunction::Print() {
  std::stringstream ss;
  ss << DirToStr() << " " << Function::Print();
  return ss.str();
}

// -----------------------------------------------------------------------------
std::string cyclus::cyclopts::ObjectiveFunction::DirToStr() {
  switch(dir_) {
    case MIN:
      return "min";
      break;
    case MAX:
      return "max";
      break;
  }
}
