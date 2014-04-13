// symbolic_function_tests.h
#ifndef CYCLUS_TESTS_SYMBOLIC_FUNCTION_TESTS_H_
#define CYCLUS_TESTS_SYMBOLIC_FUNCTION_TESTS_H_

#include <gtest/gtest.h>

#include "symbolic_functions.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SymbolicFunctionTests : public ::testing::Test
{
 public:
  cyclus::SymFunction::Ptr linear_func, exp_func;

  double lin_xoffset, lin_yoffset;
  double slope, intercept;

  double exp_xoffset, exp_yoffset;
  double constant, exponent;

  std::vector<double> check_points;

  virtual void SetUp();
  virtual void TearDown();

  void SetUpPiecewiseEnvironment();

  cyclus::SymFunction::Ptr GetLinFunction();
  cyclus::SymFunction::Ptr GetExpFunction();
  cyclus::SymFunction::Ptr GetPiecewiseFunction();

  double linear_value(double value);
  double exp_value(double value);
  double piecewise_value(double value, int index);
};

#endif  // CYCLUS_TESTS_SYMBOLIC_FUNCTION_TESTS_H_
