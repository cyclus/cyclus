#include "solver_interface.h"

#include <stdexcept>
#include <vector>
#include <sstream>
#include <iostream>

#include "cyclopts_limits.h"

using namespace std;
using namespace cyclopts;

// -----------------------------------------------------------------------------
SolverInterface::SolverInterface(SolverPtr s) : solver_(s) {
  constraints_ = vector<ConstraintPtr>();
  variables_ = vector<VariablePtr>();
  modifier_limit_ = kModifierLimit; // this is a bandaid, I don't know why it has to happen... somethings up with cbc
};

// -----------------------------------------------------------------------------
void SolverInterface::RegisterVariable(VariablePtr v) {
  variables_.push_back(v);
}

// -----------------------------------------------------------------------------
void SolverInterface::RegisterObjFunction(ObjFuncPtr obj) {
  obj_ = obj;
}

// -----------------------------------------------------------------------------
void SolverInterface::AddVarToObjFunction(VariablePtr v, double modifier) {
  // need to check that v is in variables_
  CheckModifierBounds(modifier);
  obj_->AddConstituent(v,modifier);
}

// -----------------------------------------------------------------------------
void SolverInterface::RegisterConstraint(ConstraintPtr c) {
  constraints_.push_back(c);
}

// -----------------------------------------------------------------------------
void SolverInterface::AddVarToConstraint(VariablePtr v, double modifier, 
                                         ConstraintPtr c) {
  CheckModifierBounds(modifier);
  // need to check that v is in variables_ and c is in constraints_
  vector<ConstraintPtr>::iterator it;
  it = find(constraints_.begin(),constraints_.end(),c);
  it->get()->AddConstituent(v,modifier);
}

// -----------------------------------------------------------------------------
void SolverInterface::Solve() {
  solver_->Solve(variables_,obj_,constraints_);
}

// -------------------------------------------------------------------
void SolverInterface::CheckModifierBounds(double modifier) {
  if (modifier > modifier_limit_) {
      stringstream msg;
      msg << "Cannot add modifier " 
          << modifier
          << " which is greater than the modifier limit " 
          << modifier_limit_;
      throw runtime_error(msg.str());
    }
}
