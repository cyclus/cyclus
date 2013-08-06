// inst_model_tests.cc 
#include <gtest/gtest.h>

#include "Instmodel_tests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//- - - - - - - - - - - Param per-Inst Tests  - - - - - - - - - - - - - - - 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(InstModelTests, Tick) {
  int time = 1;
  EXPECT_NO_THROW(inst_model_->HandleTick(time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(InstModelTests, Tock) {
  int time = 1;
  EXPECT_NO_THROW(inst_model_->HandleTock(time));
}

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TEST_F(InstModelTests, AddPrototype) {
//   EXPECT_FALSE( inst_model_->IsAvailablePrototype(test_facility_));
//   EXPECT_NO_THROW( inst_model_->wrapAddPrototype(test_facility_) );
//   EXPECT_TRUE( inst_model_->IsAvailablePrototype(test_facility_));
// }

