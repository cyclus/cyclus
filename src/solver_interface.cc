#include "solver_interface.h"

#include <stdexcept>
#include <vector>
#include <sstream>
#include <iostream>

#include "cyc_limits.h"

namespace cyclus {

// -----------------------------------------------------------------------------
SolverInterface::SolverInterface(Solver::Ptr s) : solver_(s) {
  constraints_ = std::vector<Constraint::Ptr>();
  variables_ = std::vector<Variable::Ptr>();
  modifier_limit_ = kModifierLimit; // this is a bandaid, I don't know why it has to happen... somethings up with cbc
};

// -----------------------------------------------------------------------------
void SolverInterface::RegisterVariable(Variable::Ptr v) {
  variables_.push_back(v);
}

// -----------------------------------------------------------------------------
void SolverInterface::RegisterObjFunction(ObjectiveFunction::Ptr obj) {
  obj_ = obj;
}

// -----------------------------------------------------------------------------
void SolverInterface::AddVarToObjFunction(Variable::Ptr v, double modifier) {
  // need to check that v is in variables_
  CheckModifierBounds(modifier);
  obj_->AddVariable(v, modifier);
}

// -----------------------------------------------------------------------------
void SolverInterface::RegisterConstraint(Constraint::Ptr c) {
  constraints_.push_back(c);
}

// -----------------------------------------------------------------------------
void SolverInterface::AddVarToConstraint(
    Variable::Ptr v, 
    double modifier, 
    Constraint::Ptr c) {
  CheckModifierBounds(modifier);
  // need to check that v is in variables_ and c is in constraints_
  std::vector<Constraint::Ptr>::iterator it;
  it = find(constraints_.begin(), constraints_.end(), c);
  it->get()->AddVariable(v, modifier);
}

// -----------------------------------------------------------------------------
void SolverInterface::Solve() {
  solver_->Solve(variables_, obj_, constraints_);
}

// -------------------------------------------------------------------
void SolverInterface::CheckModifierBounds(double modifier) {
  if (modifier > modifier_limit_) {
    std::stringstream msg;
    msg << "Cannot add modifier " 
        << modifier
        << " which is greater than the modifier limit " 
        << modifier_limit_;
    throw std::runtime_error(msg.str());
  }
}

} // namespace cyclus
