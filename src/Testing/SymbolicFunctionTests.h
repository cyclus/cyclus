// SymbolicFunctionTests.h
#include <gtest/gtest.h>

#include "SymbolicFunctions.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SymbolicFunctionTests : public ::testing::Test 
{
 public:
  FunctionPtr linear_func, exp_func;
  
  double lin_xoffset, lin_yoffset, exp_xoffset, exp_yoffset;
  
  virtual void SetUp();  
  virtual void TearDown();
};
