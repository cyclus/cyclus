#include "cbc_solver.h"

#include <iostream>
#include <utility>

#include <boost/any.hpp>

// Coin related
#include "CoinModel.hpp"
#include "CbcModel.hpp"
#include "OsiClpSolverInterface.hpp"

#include "cyclopts/limits.h"

// -----------------------------------------------------------------------------
std::pair<double, double> cyclus::cyclopts::CBCSolver::ConstraintBounds(
    cyclus::cyclopts::ConstraintPtr c) {
  double lval, rval;
  switch(c->eq_relation()) {
    case Constraint::EQ:
      lval = c->rhs(); 
      rval = c->rhs();
      break;
    case Constraint::GTEQ:
      lval = c->rhs(); 
      rval = COIN_DBL_MAX;
    case Constraint::GT: // explicit fall through
      lval += kConstraintEps; 
      break;
    case Constraint::LTEQ:
      lval = COIN_DBL_MAX; 
      rval = c->rhs();
    case Constraint::LT: // explicit fall through
      rval -= cyclus::cyclopts::kConstraintEps; 
      break;
  }
  return std::pair<double, double>(lval, rval);
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::CBCSolver::SetUpVariablesAndObj(
    std::vector<cyclus::cyclopts::VariablePtr>& variables, 
    cyclus::cyclopts::ObjFuncPtr obj) {
  for (int i = 0; i < variables.size(); i++) {
    cyclus::cyclopts::VariablePtr v = variables.at(i);
    std::pair<int, int> ibounds;
    std::pair<double, double> lbounds;
    switch(v->type()) {
      case cyclus::cyclopts::Variable::INT:
        ibounds = cyclus::cyclopts::GetIntBounds(v);
        builder_.setColumnBounds(i, ibounds.first, ibounds.second);
        builder_.setInteger(i);
        break;
      case cyclus::cyclopts::Variable::LINEAR:
        lbounds = cyclus::cyclopts::GetLinBounds(v);
        builder_.setColumnBounds(i, lbounds.first, lbounds.second);
        break;
    }
    builder_.setObjective(i, obj->GetModifier(v));
  }
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::CBCSolver::SetUpConstraints(
    std::vector<cyclus::cyclopts::ConstraintPtr>& constraints) {
  for (int i = 0; i < constraints.size(); i++) {
    cyclus::cyclopts::ConstraintPtr c = constraints.at(i);
    std::pair<double, double> bounds = ConstraintBounds(c);
    builder_.setRowBounds(i, bounds.first, bounds.second);
    std::map<cyclus::cyclopts::VariablePtr, double>::const_iterator it;
    for (it = c->begin(); it != c->end(); ++it) {
      builder_.setElement(i, index_[it->first], it->second);
    }
  }
}

// -----------------------------------------------------------------------------
double cyclus::cyclopts::CBCSolver::ObjDirection(
    cyclus::cyclopts::ObjFuncPtr obj) {
  double sense_value;
  switch(obj->dir()) {
    case ObjectiveFunction::MIN:
      sense_value = 1.0;
      break;
    case ObjectiveFunction::MAX:
      sense_value = -1.0;
      break;
  }
  return sense_value;
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::CBCSolver::SolveModel(CbcModel& model) {
  model.messageHandler()->setLogLevel(0); // turn off all output
  model.solver()->messageHandler()->setLogLevel(0); // turn off all output
  model.branchAndBound();
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::CBCSolver::PopulateSolution(
    CbcModel& model, 
    std::vector<cyclus::cyclopts::VariablePtr>& variables) {
  int ncol = model.solver()->getNumCols();
  const double* solution = model.solver()->getColSolution();
  
  for (int i = 0; i < variables.size(); i++) {
    boost::any value = solution[i]; 
    switch(variables.at(i)->type()) { 
      case cyclus::cyclopts::Variable::INT:
        value = static_cast<int>(solution[i]);
        break;
    }
    variables.at(i)->set_value(value);
  }
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::CBCSolver::PrintVariables(int num) {
  std::cout << "Variables:" << std::endl;
  for (int i = 0; i < num; i++) {
    std::cout << "  lbound: " << builder_.getColLower(i) << " ubound: " << 
      builder_.getColUpper(i) << " integer: " 
         << builder_.getColIsInteger(i) << std::endl;
  }
  std::cout << "note true = " << true << std::endl;
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::CBCSolver::PrintObjFunction(int num) {
  std::cout << "Objective Function:" << std::endl;
  std::cout << "  direction: " << builder_.optimizationDirection() << std::endl;
  std::cout << "    values: ";
  for (int i = 0; i < num; i++) {
    std::cout << builder_.getColumnObjective(i) <<  " ";
  }
  std::cout << std::endl;
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::CBCSolver::PrintConstraints(int n_const, int n_vars) {
  std::cout << "Constraints:" << std::endl;
  for (int i = 0; i < n_const; i++) {
    std::cout << "  lbound: " << builder_.getRowLower(i) << " ubound: " << 
      builder_.getRowUpper(i) << std::endl;
    std::cout << "    values: ";
    for (int j = 0; j < n_vars; j++) {
      std::cout << builder_.getElement(i, j) << " ";
    }
    std::cout << std::endl;
  }
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::CBCSolver::Print(int n_const, int n_vars) {
  PrintVariables(n_vars);
  PrintObjFunction(n_vars);
  PrintConstraints(n_const, n_vars);
}

// -----------------------------------------------------------------------------
void cyclus::cyclopts::CBCSolver::Solve(
    std::vector<cyclus::cyclopts::VariablePtr>& variables, 
    cyclus::cyclopts::ObjFuncPtr obj, 
    std::vector<cyclus::cyclopts::ConstraintPtr>& constraints) {
  // use builder_ to build constraint probelm
  Solver::PopulateIndices(variables);
  SetUpConstraints(constraints);
  SetUpVariablesAndObj(variables, obj);
  builder_.setOptimizationDirection(ObjDirection(obj));
  // Print(constraints.size(),variables.size());

  // solve constraint problem
  OsiClpSolverInterface solver1;
  OsiSolverInterface* solver = &solver1;
  solver->loadFromCoinModel(builder_);
  CbcModel model(*solver);
  SolveModel(model);
  PopulateSolution(model, variables);
}
