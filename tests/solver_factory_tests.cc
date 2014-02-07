#include <gtest/gtest.h>

#include "solver_factory.h"

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
  double obj_coeff [] = {2.0, 0.5, 1.8};
  si->setObjective(obj_coeff);
  si->setObjSense(1.0); // min
  double col_lower [] = {1.3, 2.0, 0.4};
  si->setColLower(col_lower);
  double col_upper [] = {inf, inf, inf};
  si->setColUpper(col_upper);
  CoinPackedVector row1;
  row1.insert(0, 1); // x
  row1.insert(1, 1); // y
  si->addRow(row1, 4.4, inf);
  CoinPackedVector row2;
  row2.insert(1, 1); // y
  row2.insert(2, 1); // z
  si->addRow(row2, -inf, 3.1);
  si->setInteger(1); // y
  si->setInteger(2); // z
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SolverFactoryTests::Solve(OsiSolverInterface* si) {
  si->initialSolve();
  si->branchAndBound();
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
