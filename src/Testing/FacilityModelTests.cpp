// FacilityModelTests.cpp 
#include <gtest/gtest.h>

#include "FacilityModelTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(FacilityModelTests, DISABLED_Tick) {
  int time = 1;
  facility_model_->HandleTick(time);
    //EXPECT_NO_THROW();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(FacilityModelTests, DISABLED_Tock) {
  int time = 1;
  EXPECT_NO_THROW(facility_model_->HandleTock(time));
}
