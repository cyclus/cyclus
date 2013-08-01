#include "function.h"

#include <utility>
#include <map>
#include <string>
#include <sstream>

#include "variable.h"

using namespace std;
using namespace cyclopts;

// -----------------------------------------------------------------------------
Function::Function() {
  constituents_ = map<VariablePtr,double>();
}

// -----------------------------------------------------------------------------
void Function::AddConstituent(VariablePtr v, double modifier) {
  constituents_.insert(pair<VariablePtr,double>(v,modifier));
}

// -----------------------------------------------------------------------------
double Function::GetModifier(VariablePtr v) {
  return constituents_[v];
}

// -----------------------------------------------------------------------------
std::map<VariablePtr,double>::iterator Function::begin() {
  return constituents_.begin();
}

// -----------------------------------------------------------------------------
std::map<VariablePtr,double>::iterator Function::end() {
  return constituents_.end();
}

// -----------------------------------------------------------------------------
int Function::NumConstituents() { 
  return constituents_.size(); 
}

// -----------------------------------------------------------------------------
std::string Function::Print() { 
  stringstream ss;
  int count = 0;
  map<VariablePtr,double>::iterator it;
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
    : eq_relation_(eq_r), rhs_(rhs) {};

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
  stringstream ss;
  ss << "s.t. " << Function::Print() << " " << EqRToStr() << " " << rhs_;
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
  }
}

// -----------------------------------------------------------------------------
ObjectiveFunction::ObjectiveFunction(ObjectiveFunction::Direction dir)
    : dir_(dir) {};

// -----------------------------------------------------------------------------
ObjectiveFunction::Direction ObjectiveFunction::dir() {
  return dir_;
}

// -----------------------------------------------------------------------------
std::string ObjectiveFunction::Print() {
  stringstream ss;
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
  }
}
