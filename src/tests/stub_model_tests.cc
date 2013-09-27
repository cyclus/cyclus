// Stubmodel_tests.cc 
#include <gtest/gtest.h>

#include "stub_model_tests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(StubModelTests, Print) {
  int time = 1;
  EXPECT_NO_THROW(std::string s = stub_model_->str());
}

