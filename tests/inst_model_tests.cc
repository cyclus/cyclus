// inst_model_tests.cc 
#include <gtest/gtest.h>

#include "inst_model_tests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//- - - - - - - - - - - Param per-Inst Tests  - - - - - - - - - - - - - - - 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(InstitutionTests, Tick) {
  int time = 1;
  EXPECT_NO_THROW(inst_model_->Tick(time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(InstitutionTests, Tock) {
  int time = 1;
  EXPECT_NO_THROW(inst_model_->Tock(time));
}

