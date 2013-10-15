#include "solver_interface.h"

#include <stdexcept>
#include <vector>
#include <sstream>
#include <iostream>

#include "limits.h"

// -----------------------------------------------------------------------------
cyclus::optim::SolverInterface::SolverInterface(SolverPtr s) : solver_(s) {
  constraints_ = std::vector<cyclus::optim::ConstraintPtr>();
  variables_ = std::vector<cyclus::optim::VariablePtr>();
  modifier_limit_ = kModifierLimit; // this is a bandaid, I don't know why it has to happen... somethings up with cbc
};

// -----------------------------------------------------------------------------
void cyclus::optim::SolverInterface::RegisterVariable(
    cyclus::optim::VariablePtr v) {
  variables_.push_back(v);
}

// -----------------------------------------------------------------------------
void cyclus::optim::SolverInterface::RegisterObjFunction(
    cyclus::optim::ObjFuncPtr obj) {
  obj_ = obj;
}

// -----------------------------------------------------------------------------
void cyclus::optim::SolverInterface::AddVarToObjFunction(
    cyclus::optim::VariablePtr v, 
    double modifier) {
  // need to check that v is in variables_
  CheckModifierBounds(modifier);
  obj_->AddVariable(v, modifier);
}

// -----------------------------------------------------------------------------
void cyclus::optim::SolverInterface::RegisterConstraint(
    cyclus::optim::ConstraintPtr c) {
  constraints_.push_back(c);
}

// -----------------------------------------------------------------------------
void cyclus::optim::SolverInterface::AddVarToConstraint(
    cyclus::optim::VariablePtr v, 
    double modifier, 
    cyclus::optim::ConstraintPtr c) {
  CheckModifierBounds(modifier);
  // need to check that v is in variables_ and c is in constraints_
  std::vector<cyclus::optim::ConstraintPtr>::iterator it;
  it = find(constraints_.begin(), constraints_.end(), c);
  it->get()->AddVariable(v, modifier);
}

// -----------------------------------------------------------------------------
void cyclus::optim::SolverInterface::Solve() {
  solver_->Solve(variables_, obj_, constraints_);
}

// -------------------------------------------------------------------
void cyclus::optim::SolverInterface::CheckModifierBounds(double modifier) {
  if (modifier > modifier_limit_) {
    std::stringstream msg;
    msg << "Cannot add modifier " 
        << modifier
        << " which is greater than the modifier limit " 
        << modifier_limit_;
    throw std::runtime_error(msg.str());
  }
}
