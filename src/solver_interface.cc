#include "solver_interface.h"

#include <stdexcept>
#include <vector>
#include <sstream>
#include <iostream>

#include "cyc_limits.h"

// -----------------------------------------------------------------------------
cyclus::SolverInterface::SolverInterface(SolverPtr s) : solver_(s) {
  constraints_ = std::vector<cyclus::ConstraintPtr>();
  variables_ = std::vector<cyclus::VariablePtr>();
  modifier_limit_ = cyclus::kModifierLimit; // this is a bandaid, I don't know why it has to happen... somethings up with cbc
};

// -----------------------------------------------------------------------------
void cyclus::SolverInterface::RegisterVariable(
    cyclus::VariablePtr v) {
  variables_.push_back(v);
}

// -----------------------------------------------------------------------------
void cyclus::SolverInterface::RegisterObjFunction(
    cyclus::ObjFuncPtr obj) {
  obj_ = obj;
}

// -----------------------------------------------------------------------------
void cyclus::SolverInterface::AddVarToObjFunction(
    cyclus::VariablePtr v, 
    double modifier) {
  // need to check that v is in variables_
  CheckModifierBounds(modifier);
  obj_->AddVariable(v, modifier);
}

// -----------------------------------------------------------------------------
void cyclus::SolverInterface::RegisterConstraint(
    cyclus::ConstraintPtr c) {
  constraints_.push_back(c);
}

// -----------------------------------------------------------------------------
void cyclus::SolverInterface::AddVarToConstraint(
    cyclus::VariablePtr v, 
    double modifier, 
    cyclus::ConstraintPtr c) {
  CheckModifierBounds(modifier);
  // need to check that v is in variables_ and c is in constraints_
  std::vector<cyclus::ConstraintPtr>::iterator it;
  it = find(constraints_.begin(), constraints_.end(), c);
  it->get()->AddVariable(v, modifier);
}

// -----------------------------------------------------------------------------
void cyclus::SolverInterface::Solve() {
  solver_->Solve(variables_, obj_, constraints_);
}

// -------------------------------------------------------------------
void cyclus::SolverInterface::CheckModifierBounds(double modifier) {
  if (modifier > modifier_limit_) {
    std::stringstream msg;
    msg << "Cannot add modifier " 
        << modifier
        << " which is greater than the modifier limit " 
        << modifier_limit_;
    throw std::runtime_error(msg.str());
  }
}
