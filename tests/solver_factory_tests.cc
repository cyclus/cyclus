#include <gtest/gtest.h>

#include "CoinMessageHandler.hpp"
#include "CoinPackedMatrix.hpp"
#include "CoinPackedVector.hpp"
#include "OsiSolverInterface.hpp"

#include "equality_helpers.h"
#include "solver_factory.h"

namespace cyclus {

class SolverFactoryTests : public ::testing::Test  {
 public:
  virtual void SetUp();
  virtual void TearDown();
  void Init(OsiSolverInterface* si);
  void InitRedundant(OsiSolverInterface* si);

 protected:
  SolverFactory sf_;

  int n_vars_;
  int n_int_vars_;
  int n_rows_;

  double lp_obj_;
  double* lp_exp_;

  double mip_obj_;
  double* mip_exp_;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SolverFactoryTests::SetUp() {
  n_vars_ = 3;
  n_int_vars_ = 2;
  n_rows_ = 2;
  lp_obj_ = 5.47;
  lp_exp_ = new double[n_vars_];
  lp_exp_[0] = 1.7; lp_exp_[1] = 2.7; lp_exp_[2] = 0.4;
  mip_obj_ = 7.6;
  mip_exp_ = new double[n_vars_];
  mip_exp_[0] = 2.4; mip_exp_[1] = 2; mip_exp_[2] = 1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SolverFactoryTests::TearDown() {
  delete [] lp_exp_;
  delete [] mip_exp_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SolverFactoryTests::Init(OsiSolverInterface* si) {
  // min  2x + 0.5y + 1.8z
  // s.t.  x +    y        > 4.4
  //              y +    z < 3.1
  //     x > 1.3, y > 2, z > 0.4
  //                y, z integer (if integer solver)
  double inf = si->getInfinity();
  double obj [] = {2.0, 0.5, 1.8};
  double col_lb [] = {1.3, 2.0, 0.4};
  double col_ub [] = {inf, inf, inf};
  double row_lb [] = {4.4, -1.0*inf};
  double row_ub [] = {inf, 3.1};
  CoinPackedVector row1;
  row1.insert(0, 1.0);  // x
  row1.insert(1, 1.0);  // y
  CoinPackedVector row2;
  row2.insert(1, 1);  // y
  row2.insert(2, 1);  // z
  CoinPackedMatrix m(false, 0, 0);
  m.setDimensions(0, n_vars_);
  m.appendRow(row1);
  m.appendRow(row2);
  si->loadProblem(m, &col_lb[0], &col_ub[0], &obj[0], &row_lb[0], &row_ub[0]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SolverFactoryTests::InitRedundant(OsiSolverInterface* si) {
  // min  x + y
  // s.t. x + y >= 1
  //      x, y in [0, 1], integer if integer solver
  double inf = si->getInfinity();
  double obj [] = {1.0, 1.0};
  double col_lb [] = {0.0, 0.0};
  double col_ub [] = {1.0, 1.0};
  double row_lb [] = {1.0};
  double row_ub [] = {inf};
  CoinPackedVector row1;
  row1.insert(0, 1.0);  // x
  row1.insert(1, 1.0);  // y
  CoinPackedMatrix m(false, 0, 0);
  m.setDimensions(0, 2);
  m.appendRow(row1);
  si->loadProblem(m, &col_lb[0], &col_ub[0], &obj[0], &row_lb[0], &row_ub[0]);
}

TEST_F(SolverFactoryTests, Clp) {
  sf_.solver_t("clp");
  OsiSolverInterface* si = sf_.get();
  CoinMessageHandler h;
  h.setLogLevel(0);
  si->passInMessageHandler(&h);
  Init(si);
  SolveProg(si);
  const double* exp = &lp_exp_[0];
  array_double_eq(&exp[0], si->getColSolution(), n_vars_);
  EXPECT_DOUBLE_EQ(lp_obj_, si->getObjValue());
  delete si;
}

TEST_F(SolverFactoryTests, ClpRedundant) {
  sf_.solver_t("clp");
  OsiSolverInterface* si = sf_.get();
  CoinMessageHandler h;
  h.setLogLevel(0);
  si->passInMessageHandler(&h);
  InitRedundant(si);
  SolveProg(si);
  double exp [] = {1, 0};
  array_double_eq(exp, si->getColSolution(), 2);
  EXPECT_DOUBLE_EQ(1, si->getObjValue());
  delete si;
}

TEST_F(SolverFactoryTests, Cbc) {
  sf_.solver_t("cbc");
  OsiSolverInterface* si = sf_.get();
  CoinMessageHandler h;
  h.setLogLevel(0);
  si->passInMessageHandler(&h);
  Init(si);
  si->setInteger(1);  // y
  si->setInteger(2);  // z
  SolveProg(si);
  const double* exp = &mip_exp_[0];
  array_double_eq(&exp[0], si->getColSolution(), n_vars_);
  EXPECT_DOUBLE_EQ(mip_obj_, si->getObjValue());
}

TEST_F(SolverFactoryTests, CbcRedundant) {
  sf_.solver_t("clp");
  OsiSolverInterface* si = sf_.get();
  CoinMessageHandler h;
  h.setLogLevel(0);
  si->passInMessageHandler(&h);
  InitRedundant(si);
  si->setInteger(0);  // x
  si->setInteger(1);  // y
  SolveProg(si);
  double exp [] = {1, 0};
  array_double_eq(exp, si->getColSolution(), 2);
  EXPECT_DOUBLE_EQ(1, si->getObjValue());
  delete si;
}

}  // namespace cyclus
