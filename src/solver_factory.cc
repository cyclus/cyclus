#include "solver_factory.h"

#include "OsiClpSolverInterface.hpp"
#include "OsiCbcSolverInterface.hpp" 

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OsiSolverInterface* SolverFactory::get() {
  if (t_ == "clp") {
    return new OsiClpSolverInterface();
  } else if (t_ == "cbc") {
    return new OsiCbcSolverInterface();
  }
}

} // namespace cyclus
