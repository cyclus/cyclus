#include <gtest/gtest.h>

#include <math.h>
#include <limits>

#include "SymbolicFunctionTests.h"
#include "SymbolicFunctionFactories.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SymbolicFunctionTests::SetUp()
{
  slope = 2.0;
  intercept = 1.0;
  lin_xoffset = 0.0;
  lin_yoffset = 0.0;

  constant = 3.0;
  exponent = 0.25;
  exp_xoffset = 0.0;
  exp_yoffset = 0.0;
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SymbolicFunctionTests::TearDown() 
{
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SymbolicFunctionTests::SetUpPiecewiseEnvironment()
{
  double offset = 4;

  // point 0, straight line at y=0
  check_points.push_back(0.0);

  // point 1, interface with straight line and linear function
  check_points.push_back(offset);
  intercept = 0.0;
  lin_yoffset = 0.0;
  lin_xoffset = offset;

  // point 2, linear function
  check_points.push_back(offset*1.5);

  // point 3, interface with linear function and exp function
  check_points.push_back(offset*2);
  exp_xoffset = offset*2;
  exp_yoffset = linear_value(offset) + lin_yoffset - exp_value(0);  

  // point 3, exp function
  check_points.push_back(offset*3);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
cyclus::FunctionPtr SymbolicFunctionTests::GetExpFunction() 
{
  cyclus::ExpFunctionFactory eff;
  std::stringstream input;
  input << constant << " " << exponent;
  return eff.GetFunctionPtr(input.str());
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
cyclus::FunctionPtr SymbolicFunctionTests::GetLinFunction() 
{
  cyclus::LinFunctionFactory lff;
  std::stringstream input;
  input << slope << " " << intercept;
  return lff.GetFunctionPtr(input.str());
}  


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
cyclus::FunctionPtr SymbolicFunctionTests::GetPiecewiseFunction()
{
  SetUpPiecewiseEnvironment();
  cyclus::FunctionPtr lin = GetLinFunction();
  cyclus::FunctionPtr exp = GetExpFunction();
  
  cyclus::PiecewiseFunctionFactory pff;
  pff.AddFunction(lin,check_points.at(1));
  pff.AddFunction(exp,check_points.at(3));

  return pff.GetFunctionPtr();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double SymbolicFunctionTests::linear_value(double value)
{
  return slope * value + intercept;
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double SymbolicFunctionTests::exp_value(double value)
{
  return constant * exp(exponent * value);
}  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double SymbolicFunctionTests::piecewise_value(double value, int index)
{
  double ret = -1;
  switch(index)
    {
    case(0):
      ret = 0;
      break;
    case(1):
    case(2):
      ret = linear_value(value-lin_xoffset) + lin_yoffset;
      break;
    case(3):
    case(4):
      ret = exp_value(value-exp_xoffset) + exp_yoffset;
      break;
    }
  return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SymbolicFunctionTests,linearfunc) 
{
  cyclus::FunctionPtr f = GetLinFunction();
  
  int n = 10;
  int start = -10;
  double step = 0.5;

  for (int i = start; i < n; i++)
    {
      double x = i*step;
      EXPECT_DOUBLE_EQ(linear_value(x),f->value(x));
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SymbolicFunctionTests,expfunc) 
{
  cyclus::FunctionPtr f = GetExpFunction();
  
  int n = 10;
  int start = -10;
  double step = 0.5;

  for (int i = start; i < n; i++)
    {
      double x = i*step;
      EXPECT_DOUBLE_EQ(exp_value(x),f->value(x));
    }
}

//#include <iostream>
//#include <fstream>
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SymbolicFunctionTests,piecewisefunc) 
{
  //ofstream output;
  //output.open ("out");

  cyclus::FunctionPtr f = GetPiecewiseFunction();

  for (int i = 0; i < check_points.size() - 1; i++)
    {
      int n = 50;
      double eps = 0.00000001;
      double range = check_points.at(i+1) - check_points.at(i) - eps;
      double step = range/(n-1);

      for (int j = 0; j < n; j++)
        {
          double x = j*step + check_points.at(i);
          EXPECT_DOUBLE_EQ(piecewise_value(x,i),f->value(x));
          //output << x << ", " << piecewise_value(x,i) << ", " << f->value(x) << endl;
        }
    }
  //output.close();
}
