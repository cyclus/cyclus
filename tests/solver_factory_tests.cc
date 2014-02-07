#include <gtest/gtest.h>

#include "solver_factory.h"

#include "CoinPackedMatrix.hpp"
#include "CoinPackedVector.hpp"
#include "OsiSolverInterface.hpp"

#include "equality_helpers.h"

class SolverFactoryTests : public ::testing::Test  {
 public:
  virtual void SetUp();
  virtual void TearDown();
  void Init(OsiSolverInterface* si);
  void Solve(OsiSolverInterface* si);

 protected:
  SolverFactory sf_;
  
  int n_vars_;
  int n_int_vars_;
  int n_rows_;
  
  double lp_obj_;
  double* lp_exp_;

  double mip_obj_;
  double* mip_exp_c_;
  int* mip_exp_i_;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SolverFactoryTests::SetUp() {
  n_vars_ = 3;
  n_int_vars_ = 2;
  n_rows_ = 2;
  lp_obj_ = 5.47;
  lp_exp_ = new double[n_vars_];
  lp_exp_[0] = 1.7; lp_exp_[1] = 2.7; lp_exp_[2] = 0.4;
  mip_exp_c_ = new double[n_vars_ - n_int_vars_];
  mip_exp_c_[0] = 1.5; 
  mip_exp_i_ = new int[n_int_vars_];
  mip_exp_i_[0] = 0; mip_exp_i_[1] = 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SolverFactoryTests::TearDown() {
  delete [] lp_exp_;
  delete [] mip_exp_c_;
  delete [] mip_exp_i_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SolverFactoryTests::Init(OsiSolverInterface* si) {
  // min  2x + 0.5y + 1.8z
  // s.t.  x +    y        > 4.4
  //              y +    z < 3.1
  //     x > 1.3, y > 2, z > 0.4
  //                y, z integer
  double inf = si->getInfinity();
  double obj [] = {2.0, 0.5, 1.8};
  // si->setObjective(obj_coeff);
  // si->setObjSense(1.0); // min
  double col_lb [] = {1.3, 2.0, 0.4};
  double col_ub [] = {inf, inf, inf};
  double row_lb [] = {4.4, -1.0*inf};
  double row_ub [] = {inf, 3.1};
  // si->setColLower(col_lower);
  // si->setColUpper(col_upper);
  CoinPackedVector row1;
  row1.setElement(0, 1.0); // x
  row1.setElement(1, 1.0); // y
  CoinPackedVector row2;
  row2.setElement(1, 1); // y
  row2.setElement(2, 1); // z
  CoinPackedMatrix m;
  m.appendRow(row1);
  m.appendRow(row2);
  // si->setInteger(1); // y
  // si->setInteger(2); // z
  si->loadProblem(m, &col_lb[0], &col_ub[0], &obj[0], &row_lb[0], &row_ub[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SolverFactoryTests::Solve(OsiSolverInterface* si) {
  si->initialSolve();
  si->branchAndBound();
}

#include "OsiClpSolverInterface.hpp"

TEST_F(SolverFactoryTests, tst) {
  OsiSolverInterface* si = new OsiClpSolverInterface();
  
  int n_cols = 2;
  double *objective    = new double[n_cols];//the objective coefficients
  double *col_lb       = new double[n_cols];//the column lower bounds
  double *col_ub       = new double[n_cols];//the column upper bounds

  //Define the objective coefficients.
  //minimize -1 x0 - 1 x1
  objective[0] = -1.0;
  objective[1] = -1.0;

  //Define the variable lower/upper bounds.
  // x0 >= 0   =>  0 <= x0 <= infinity
  // x1 >= 0   =>  0 <= x1 <= infinity
  col_lb[0] = 0.0;
  col_lb[1] = 0.0;
  col_ub[0] = si->getInfinity();
  col_ub[1] = si->getInfinity();
     
  int n_rows = 2;
  double *row_lb = new double[n_rows]; //the row lower bounds
  double *row_ub = new double[n_rows]; //the row upper bounds
     
  //Define the constraint matrix.
  CoinPackedMatrix *matrix =  new CoinPackedMatrix(false,0,0);
  matrix->setDimensions(0, n_cols);

  //1 x0 + 2 x1 <= 3  =>  -infinity <= 1 x0 + 2 x2 <= 3
  CoinPackedVector row1;
  row1.insert(0, 1.0);
  row1.insert(1, 2.0);
  row_lb[0] = -1.0 * si->getInfinity();
  row_ub[0] = 3.0;
  matrix->appendRow(row1);

  //2 x0 + 1 x1 <= 3  =>  -infinity <= 2 x0 + 1 x1 <= 3
  CoinPackedVector row2;
  row2.insert(0, 2.0);
  row2.insert(1, 1.0);
  row_lb[1] = -1.0 * si->getInfinity();
  row_ub[1] = 3.0;
  matrix->appendRow(row2);

  //load the problem to OSI
  si->loadProblem(*matrix, col_lb, col_ub, objective, row_lb, row_ub);
  delete si;
}

TEST_F(SolverFactoryTests, Clp) {
  sf_.solver_t("clp");
  OsiSolverInterface* si = sf_.get();
  Init(si);
  // Solve(si);
  // const double* exp = &lp_exp_[0];
  // array_double_eq(&exp[0], si->getColSolution(), n_vars_);
  // EXPECT_DOUBLE_EQ(lp_obj_, si->getObjValue());
  delete si;
}

// TEST_F(SolverFactoryTests, Cbc) {
//   sf_.solver_t("cbc");
//   OsiSolverInterface* si = sf_.get();
//   Init(si);
//   Solve(si);
  
// }
