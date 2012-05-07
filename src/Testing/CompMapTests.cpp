// CompMapTests.cpp 
#include <gtest/gtest.h>

#include "CompMapTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,default_constructor) {
  EXPECT_EQ(comp.basis(),basis);
  EXPECT_EQ(map,comp.map());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CompMapTests,copy_constructor) {
  CompMap copy = CompMap(comp);
  EXPECT_EQ(copy.basis(),comp.basis());
  EXPECT_EQ(copy.map(),comp.map());
}
