#include "function.h"

#include <utility>
#include <map>
#include <string>
#include <sstream>

#include "variable.h"
#include "error.h"

// -----------------------------------------------------------------------------
cyclus::optim::Function::Function() {
  constituents_ = std::map<cyclus::optim::VariablePtr, double>();
}

// -----------------------------------------------------------------------------
void cyclus::optim::Function::AddVariable(cyclus::optim::VariablePtr v,
                                             double modifier) {
  constituents_.insert(
      std::pair<cyclus::optim::VariablePtr, double>(v, modifier));
}

// -----------------------------------------------------------------------------
double cyclus::optim::Function::GetModifier(cyclus::optim::VariablePtr v) {
  return constituents_[v];
}

// -----------------------------------------------------------------------------
std::map<cyclus::optim::VariablePtr, double>::const_iterator 
cyclus::optim::Function::begin() {
  return constituents_.begin();
}

// -----------------------------------------------------------------------------
std::map<cyclus::optim::VariablePtr, double>::const_iterator 
cyclus::optim::Function::end() {
  return constituents_.end();
}

// -----------------------------------------------------------------------------
int cyclus::optim::Function::NumVars() { 
  return constituents_.size(); 
}

// -----------------------------------------------------------------------------
std::string cyclus::optim::Function::Print() { 
  std::stringstream ss;
  int count = 0;
  std::map<cyclus::optim::VariablePtr, double>::iterator it;
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
cyclus::optim::Constraint::Constraint(
    cyclus::optim::Constraint::EqualityRelation eq_r, 
    double rhs)
    : eq_relation_(eq_r), 
      rhs_(rhs) 
{ };

// -----------------------------------------------------------------------------
cyclus::optim::Constraint::EqualityRelation 
cyclus::optim::Constraint::eq_relation() { 
  return eq_relation_; 
}

// -----------------------------------------------------------------------------
double cyclus::optim::Constraint::rhs() { 
  return rhs_; 
}

// -----------------------------------------------------------------------------
std::string cyclus::optim::Constraint::Print() {
  std::stringstream ss;
  ss << "s.t. " << cyclus::optim::Function::Print() 
     << " " << EqRToStr() << " " << rhs_;
  return ss.str();
}

// -----------------------------------------------------------------------------
std::string cyclus::optim::Constraint::EqRToStr() {
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
cyclus::optim::ObjectiveFunction::ObjectiveFunction(
    cyclus::optim::ObjectiveFunction::Direction dir) : dir_(dir) 
{ }

// -----------------------------------------------------------------------------
cyclus::optim::ObjectiveFunction::Direction 
cyclus::optim::ObjectiveFunction::dir() {
  return dir_;
}

// -----------------------------------------------------------------------------
std::string cyclus::optim::ObjectiveFunction::Print() {
  std::stringstream ss;
  ss << DirToStr() << " " << Function::Print();
  return ss.str();
}

// -----------------------------------------------------------------------------
std::string cyclus::optim::ObjectiveFunction::DirToStr() {
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
