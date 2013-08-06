// facility_model_tests.cc 
#include <gtest/gtest.h>

#include "Facilitymodel_tests.h"

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
