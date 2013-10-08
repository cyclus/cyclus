// facility_model_tests.cc 
#include <gtest/gtest.h>

#include "facility_model_tests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(FacilityModelTests, Tick) {
  int time = 1;
  facility_model_->HandleTick(time);
    //EXPECT_NO_THROW();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(FacilityModelTests, Tock) {
  int time = 1;
  EXPECT_NO_THROW(facility_model_->HandleTock(time));
}
