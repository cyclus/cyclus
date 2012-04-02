// InstModelTests.cpp 
#include <gtest/gtest.h>

#include "InstModelTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(InstModelTests, CopyInst) {
  InstModel* new_inst = inst_model_;
  EXPECT_NO_THROW(new_inst->copy(inst_model_)); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(InstModelTests, AddPrototype) {
  EXPECT_EQ( inst_model_->isAvailablePrototype(test_facility_), false );
  EXPECT_NO_THROW( inst_model_->wrapAddPrototype(test_facility_) );
  EXPECT_EQ( inst_model_->isAvailablePrototype(test_facility_), true );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(InstModelTests, Tick) {
  int time = 1;
  EXPECT_NO_THROW(inst_model_->handleTick(time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(InstModelTests, Tock) {
  int time = 1;
  EXPECT_NO_THROW(inst_model_->handleTock(time));
}
