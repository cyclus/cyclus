#include "solver_factory.h"

#include <iostream>

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

void ReportProg(OsiSolverInterface* si) {
  const double* objs = si->getObjCoefficients();
  const double* clbs = si->getColLower();
  const double* cubs = si->getColUpper();
  int ncol = si->getNumCols();
  std::cout << "Column info\n";
  for (int i = 0; i != ncol; i ++) {
    std::cout << i
              << " obj" << ": " << objs[i]
              << " lb" << ": " << clbs[i]
              << " ub" << ": " << cubs[i]
              << " int" << ": " << std::boolalpha << si->isInteger(i) << '\n';
  }

  const CoinPackedMatrix* m = si->getMatrixByRow();
  const double* rlbs = si->getRowLower();
  const double* rubs = si->getRowUpper();
  int nrow = si->getNumRows();
  std::cout << "Row info\n";
  for (int i = 0; i != nrow; i ++) {
    std::cout << i
              << " lb" << ": " << rlbs[i]
              << " ub" << ": " << rubs[i] << '\n';
  }
  std::cout << "matrix:\n";
  m->dumpMatrix();
}

void SolveProg(OsiSolverInterface* si, bool verbose) {
  if (verbose)
    ReportProg(si);

  si->initialSolve();
  if (HasInt(si)) {
    OsiCbcSolverInterface* cast = dynamic_cast<OsiCbcSolverInterface*>(si);
    if (cast)
      cast->getModelPtr()->branchAndBound(); // get rid of warning
    else
      si->branchAndBound();
  } else {
    OsiClpSolverInterface* cast = dynamic_cast<OsiClpSolverInterface*>(si);
    if (cast) {
      cast->getModelPtr()->primal(); // solve problem with primal alg
    }
  }

  if (verbose) {
    const double* soln = si->getColSolution();
    for (int i = 0; i != si->getNumCols(); i ++) {
      std::cout << "soln " << i << ": " << soln[i] << '\n';
    }
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
