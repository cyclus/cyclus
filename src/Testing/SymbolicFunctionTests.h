// SymbolicFunctionTests.h
#include <gtest/gtest.h>

#include "SymbolicFunctions.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SymbolicFunctionTests : public ::testing::Test 
{
 public:
  cyclus::FunctionPtr linear_func, exp_func;
  
  double lin_xoffset, lin_yoffset;
  double slope, intercept;

  double exp_xoffset, exp_yoffset;
  double constant, exponent;

  std::vector<double> check_points;
  
  virtual void SetUp();  
  virtual void TearDown();

  void SetUpPiecewiseEnvironment();

  cyclus::FunctionPtr GetLinFunction();
  cyclus::FunctionPtr GetExpFunction();
  cyclus::FunctionPtr GetPiecewiseFunction();

  double Linear_value(double value);
  double Exp_value(double value);
  double Piecewise_value(double value, int index);
};
