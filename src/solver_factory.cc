#include "solver_factory.h"

#include <iostream>

#include "OsiClpSolverInterface.hpp"
#include "OsiCbcSolverInterface.hpp"
#include "CbcSolver.hpp"
#include "CoinTime.hpp"

#include "error.h"

namespace cyclus {

int CbcCallBack(CbcModel* model, int from) {
  int ret = 0;
  switch (from) {
    case 1:
    case 2:
      if (!model->status() && model->secondaryStatus()) ret = 1;
      break;
    case 3:
    case 4:
    case 5:
      break;
  }
  return ret;
}

ObjValueHandler::ObjValueHandler(double obj, double time, bool found)
    : obj_(obj), time_(time), found_(found) {};

ObjValueHandler::ObjValueHandler(double obj)
    : obj_(obj), time_(0), found_(false) {};

ObjValueHandler::~ObjValueHandler() {};

ObjValueHandler::ObjValueHandler(const ObjValueHandler& other)
    : CbcEventHandler(other) {
  obj_ = other.obj();
  time_ = other.time();
  found_ = other.found();
}

ObjValueHandler& ObjValueHandler::operator=(const ObjValueHandler& other) {
  if (this != &other) {
    obj_ = other.obj();
    time_ = other.time();
    found_ = other.found();
    CbcEventHandler::operator=(other);
  }
  return *this;
}

CbcEventHandler* ObjValueHandler::clone() {
  return new ObjValueHandler(*this);
}

CbcEventHandler::CbcAction ObjValueHandler::event(CbcEvent e) {
  if (!found_ && (e == solution || e == heuristicSolution)) {
    const CbcModel* m = getModel();
    double cbcobj = m->getObjValue();
    if (cbcobj < obj_) {
      time_ = CoinCpuTime() - m->getDblParam(CbcModel::CbcStartSeconds);
      found_ = true;
    }
  }
  return noAction;
}

// 10800 s = 3 hrs * 60 min/hr * 60 s/min
#define CYCLUS_SOLVER_TIMEOUT 10800

SolverFactory::SolverFactory() : t_("cbc"), tmax_(CYCLUS_SOLVER_TIMEOUT) {}
SolverFactory::SolverFactory(std::string t)
    : t_(t), tmax_(CYCLUS_SOLVER_TIMEOUT) {}
SolverFactory::SolverFactory(std::string t, double tmax) : t_(t), tmax_(tmax) {}

OsiSolverInterface* SolverFactory::get() {
  if (t_ == "clp" || t_ == "cbc") {
    OsiClpSolverInterface* s = new OsiClpSolverInterface();
    s->getModelPtr()->setMaximumSeconds(tmax_);
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
  for (int i = 0; i != ncol; i++) {
    std::cout << i << " obj" << ": " << objs[i] << " lb" << ": " << clbs[i]
              << " ub" << ": " << cubs[i] << " int" << ": " << std::boolalpha
              << si->isInteger(i) << '\n';
  }

  const CoinPackedMatrix* m = si->getMatrixByRow();
  const double* rlbs = si->getRowLower();
  const double* rubs = si->getRowUpper();
  int nrow = si->getNumRows();
  std::cout << "Row info\n";
  for (int i = 0; i != nrow; i++) {
    std::cout << i << " lb" << ": " << rlbs[i] << " ub" << ": " << rubs[i]
              << '\n';
  }
  std::cout << "matrix:\n";
  m->dumpMatrix();
}

void SolveProg(OsiSolverInterface* si, double greedy_obj, bool verbose) {
  if (verbose) ReportProg(si);

  if (HasInt(si)) {
    CbcModel model(*si);
    ObjValueHandler handler(greedy_obj);
    model.passInEventHandler(&handler);
    model.setLogLevel(0);
    model.initialSolve();
    model.branchAndBound();
    si->setColSolution(model.bestSolution());
    if (verbose) {
      std::cout << "Greedy equivalent time: " << handler.time() << " and obj "
                << handler.obj() << " and found " << std::boolalpha
                << handler.found() << "\n";
    }
  } else {
    // no ints, just solve 'initial lp relaxation'
    si->initialSolve();
  }

  if (verbose) {
    const double* soln = si->getColSolution();
    for (int i = 0; i != si->getNumCols(); i++) {
      std::cout << "soln " << i << ": " << soln[i]
                << " integer: " << std::boolalpha << si->isInteger(i) << "\n";
    }
  }
}

void SolveProg(OsiSolverInterface* si) {
  SolveProg(si, si->getInfinity(), false);
}

void SolveProg(OsiSolverInterface* si, bool verbose) {
  SolveProg(si, si->getInfinity(), verbose);
}

void SolveProg(OsiSolverInterface* si, double greedy_obj) {
  SolveProg(si, greedy_obj, false);
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
