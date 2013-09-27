#include "solver_interface.h"

#include <stdexcept>
#include <vector>
#include <sstream>
#include <iostream>

#include "limits.h"

// -----------------------------------------------------------------------------
cyclus::cyclopts::SolverInterface::SolverInterface(SolverPtr s) : solver_(s) {
  constraints_ = std::vector<cyclus::cyclopts::ConstraintPtr>();
  variables_ = std::vector<cyclus::cyclopts::VariablePtr>();
  modifier_limit_ = kModifierLimit; // this is a bandaid, I don't know why it has to happen... somethings up with cbc
};

// -----------------------------------------------------------------------------
void cyclus::cyclopts::SolverInterface::RegisterVariable(
    cyclus::cyclopts::VariablePtr v) {
  variables_.push_back(v);
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::SolverInterface::RegisterObjFunction(
    cyclus::cyclopts::ObjFuncPtr obj) {
  obj_ = obj;
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::SolverInterface::AddVarToObjFunction(
    cyclus::cyclopts::VariablePtr v, 
    double modifier) {
  // need to check that v is in variables_
  CheckModifierBounds(modifier);
  obj_->AddVariable(v, modifier);
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::SolverInterface::RegisterConstraint(
    cyclus::cyclopts::ConstraintPtr c) {
  constraints_.push_back(c);
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::SolverInterface::AddVarToConstraint(
    cyclus::cyclopts::VariablePtr v, 
    double modifier, 
    cyclus::cyclopts::ConstraintPtr c) {
  CheckModifierBounds(modifier);
  // need to check that v is in variables_ and c is in constraints_
  std::vector<cyclus::cyclopts::ConstraintPtr>::iterator it;
  it = find(constraints_.begin(), constraints_.end(), c);
  it->get()->AddVariable(v, modifier);
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::SolverInterface::Solve() {
  solver_->Solve(variables_, obj_, constraints_);
}

// -------------------------------------------------------------------
void cyclus::cyclopts::SolverInterface::CheckModifierBounds(double modifier) {
  if (modifier > modifier_limit_) {
    std::stringstream msg;
    msg << "Cannot add modifier " 
        << modifier
        << " which is greater than the modifier limit " 
        << modifier_limit_;
    throw std::runtime_error(msg.str());
  }
}
