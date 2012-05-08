// IsoVectorTests.cpp 
#include <gtest/gtest.h>

#include "IsoVectorTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,constructors) {
  IsoVector equality = IsoVector(comp);
  EXPECT_EQ(equality,vec); // equality
  IsoVector copy = IsoVector(vec);
  EXPECT_EQ(copy,vec); // copy
  IsoVector assignment = vec;
  EXPECT_EQ(assignment,vec); // assignment
}
