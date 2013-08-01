#include "cbc_solver.h"

#include <iostream>
#include <utility>

#include <boost/any.hpp>

// Coin related
#include "CoinModel.hpp"
#include "CbcModel.hpp"
#include "OsiClpSolverInterface.hpp"

#include "limits.h"

using namespace std;
using namespace boost;
using namespace cyclopts;

// -----------------------------------------------------------------------------
double CBCSolver::DoubleBound(Variable::Bound b) {
  double val;
  switch(b) {
  case Variable::NEG_INF:
    val = -COIN_DBL_MAX;
    break;
  case Variable::INF:
    val = COIN_DBL_MAX;
    break;
  }
  return val;
}

// -----------------------------------------------------------------------------
int CBCSolver::IntBound(Variable::Bound b) {
  int val;
  switch(b) {
  case Variable::NEG_INF:
    val = -COIN_INT_MAX;
    break;
  case Variable::INF:
    val = COIN_INT_MAX;
    break;
  }
  return val;
}

// -----------------------------------------------------------------------------
std::pair<double,double> CBCSolver::ConstraintBounds(ConstraintPtr c) {
  double lval, rval;
  switch(c->eq_relation()) {
  case Constraint::EQ:
    lval = c->rhs(); rval = c->rhs();
    break;
  case Constraint::GTEQ:
    lval = c->rhs(); rval = COIN_DBL_MAX;
  case Constraint::GT: // explicit fall through
    lval += kConstraintEps; 
    break;
  case Constraint::LTEQ:
    lval = COIN_DBL_MAX; rval = c->rhs();
  case Constraint::LT: // explicit fall through
    rval -= kConstraintEps; 
    break;
  }
  return pair<double,double>(lval,rval);
}

// -----------------------------------------------------------------------------
void CBCSolver::SetUpVariablesAndObj(std::vector<VariablePtr>& variables, 
                                     ObjFuncPtr obj) {
  for (int i = 0; i < variables.size(); i++) {
    VariablePtr v = variables.at(i);
    switch(v->type()) {
    case Variable::INT:
      builder_.setColumnBounds(i,IntBound(v->lbound()),
                               IntBound(v->ubound()));
      builder_.setInteger(i);
      break;
    case Variable::LINEAR:
      builder_.setColumnBounds(i,DoubleBound(v->lbound()),
                               DoubleBound(v->ubound()));
      break;
    }
    builder_.setObjective(i,obj->GetModifier(v));
  }
}

// -----------------------------------------------------------------------------
void CBCSolver::SetUpConstraints(std::vector<ConstraintPtr>& constraints) {
  for (int i = 0; i < constraints.size(); i++) {
    ConstraintPtr c = constraints.at(i);
    pair<double,double> bounds = ConstraintBounds(c);
    builder_.setRowBounds(i,bounds.first,bounds.second);
    std::map<VariablePtr,double>::iterator it;
    for (it = c->begin(); it != c->end(); it++) {
      builder_.setElement(i,index_[it->first],it->second);
    }
  }
}

// -----------------------------------------------------------------------------
double CBCSolver::ObjDirection(ObjFuncPtr obj) {
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
void CBCSolver::SolveModel(CbcModel& model) {
  model.messageHandler()->setLogLevel(0); // turn off all output
  model.solver()->messageHandler()->setLogLevel(0); // turn off all output
  model.branchAndBound();
}

// -----------------------------------------------------------------------------
void CBCSolver::PopulateSolution(CbcModel& model, 
                                 std::vector<VariablePtr>& variables) {
  int ncol = model.solver()->getNumCols();
  const double* solution = model.solver()->getColSolution();
  
  for (int i = 0; i < variables.size(); i++) {
    any value = solution[i]; 
    switch(variables.at(i)->type()) { 
    case Variable::INT:
      value = (int) solution[i];
      break;
    }
    variables.at(i)->set_value(value);
  }
}

// -----------------------------------------------------------------------------
void CBCSolver::PrintVariables(int num) {
  cout << "Variables:" << endl;
  for (int i = 0; i < num; i++) {
    cout << "  lbound: " << builder_.getColLower(i) << " ubound: " << 
      builder_.getColUpper(i) << " integer: " 
         << builder_.getColIsInteger(i) << endl;
  }
  cout << "note true = " << true << endl;
}

// -----------------------------------------------------------------------------
void CBCSolver::PrintObjFunction(int num) {
  cout << "Objective Function:" << endl;
  cout << "  direction: " << builder_.optimizationDirection() << endl;
  cout << "    values: ";
  for (int i = 0; i < num; i++) {
    cout << builder_.getColumnObjective(i) <<  " ";
  }
  cout << endl;
}

// -----------------------------------------------------------------------------
void CBCSolver::PrintConstraints(int n_const, int n_vars) {
  cout << "Constraints:" << endl;
  for (int i = 0; i < n_const; i++) {
    cout << "  lbound: " << builder_.getRowLower(i) << " ubound: " << 
      builder_.getRowUpper(i) << endl;
    cout << "    values: ";
    for (int j = 0; j < n_vars; j++) {
      cout << builder_.getElement(i,j) << " ";
    }
    cout << endl;
  }
}

// -----------------------------------------------------------------------------
void CBCSolver::Print(int n_const, int n_vars) {
  PrintVariables(n_vars);
  PrintObjFunction(n_vars);
  PrintConstraints(n_const,n_vars);
}

// -----------------------------------------------------------------------------
void CBCSolver::Solve(std::vector<VariablePtr>& variables, ObjFuncPtr obj, 
                      std::vector<ConstraintPtr>& constraints) {
  // use builder_ to build constraint probelm
  Solver::PopulateIndices(variables);
  SetUpConstraints(constraints);
  SetUpVariablesAndObj(variables,obj);
  builder_.setOptimizationDirection(ObjDirection(obj));
  // Print(constraints.size(),variables.size());

  // solve constraint problem
  OsiClpSolverInterface solver1;
  OsiSolverInterface* solver = &solver1;
  solver->loadFromCoinModel(builder_);
  CbcModel model(*solver);
  SolveModel(model);
  PopulateSolution(model,variables);
}
