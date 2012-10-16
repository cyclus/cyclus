// SymbolicFunctionTests.h
#include <gtest/gtest.h>

#include "SymbolicFunctions.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SymbolicFunctionTests : public ::testing::Test 
{
 public:
  FunctionPtr linear_func, exp_func;
  
  double lin_xoffset, lin_yoffset;
  double slope, intercept;

  double exp_xoffset, exp_yoffset;
  double constant, exponent;

  std::vector<double> check_points;
  
  virtual void SetUp();  
  virtual void TearDown();

  void setUpPiecewiseEnvironment();

  FunctionPtr getLinFunction();
  FunctionPtr getExpFunction();
  FunctionPtr getPiecewiseFunction();

  double linear_value(double value);
  double exp_value(double value);
  double piecewise_value(double value, int index);
};
