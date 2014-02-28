#include "solver_factory.h"

#include "OsiClpSolverInterface.hpp"
#include "OsiCbcSolverInterface.hpp" 

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SolverFactory::SolverFactory(std::string t) : t_(t) { }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OsiSolverInterface* SolverFactory::get() {
  if (t_ == "clp") {
    return new OsiClpSolverInterface();
  } else if (t_ == "cbc") {
    return new OsiCbcSolverInterface();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SolveProg(OsiSolverInterface* si) {
  si->initialSolve();
  if (HasInt(si)) {
    si->branchAndBound();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool HasInt(OsiSolverInterface* si) {
  int i = 0;
  for (i = 0; i != si->getNumCols(); i++) {
    if(si->isInteger(i)) {
      return true;
    }
  }
}

} // namespace cyclus
