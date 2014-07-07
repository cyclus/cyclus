#include "solver_factory.h"

#include "OsiClpSolverInterface.hpp"
#include "OsiCbcSolverInterface.hpp"

#include "error.h"

namespace cyclus {

// 10800 s = 3 hrs * 60 min/hr * 60 s/min
SolverFactory::SolverFactory() : t_("cbc"), tmax_(10800) { }
SolverFactory::SolverFactory(std::string t) : t_(t), tmax_(10800) { }
SolverFactory::SolverFactory(std::string t, double tmax)
    : t_(t),
      tmax_(tmax) { }

OsiSolverInterface* SolverFactory::get() {
  if (t_ == "clp") {
    OsiClpSolverInterface* s = new OsiClpSolverInterface();
    s->getModelPtr()->setMaximumSeconds(tmax_);
    return s;
  } else if (t_ == "cbc") {
    OsiCbcSolverInterface* s = new OsiCbcSolverInterface();
    s->setMaximumSeconds(tmax_);
    return s;
  } else {
    throw ValueError("invalid SolverFactory type '" + t_ + "'");
  }
}

void SolveProg(OsiSolverInterface* si) {
  si->initialSolve();
  if (HasInt(si)) {
    si->branchAndBound();
  }
}

bool HasInt(OsiSolverInterface* si) {
  int i = 0;
  for (i = 0; i != si->getNumCols(); i++) {
    if (si->isInteger(i)) {
      return true;
    }
  }
  return false;
}

}  // namespace cyclus
