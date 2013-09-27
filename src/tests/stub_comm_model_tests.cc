// StubCommmodel_tests.cc 
#include <gtest/gtest.h>

#include "stub_comm_model_tests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(StubCommModelTests, Print) {
  int time = 1;
  EXPECT_NO_THROW(std::string s = stub_comm_model_->str());
}

