// StubCommModelTests.cpp 
#include <gtest/gtest.h>

#include "StubCommModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(StubCommModelTests, Print) {
  int time = 1;
  EXPECT_NO_THROW(std::string s = stub_comm_model_->str());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(StubCommModelTests, CopyFreshStub) {
  StubCommModel* new_stub = stub_comm_model_;
  EXPECT_NO_THROW(new_stub->copyFreshModel(dynamic_cast<Model*>(stub_comm_model_))); 
}

