// FacilityModelTests.cpp 
#include <gtest/gtest.h>

#include "FacilityModelTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(FacilityModelTests, CopyFacility) {
  FacilityModel* new_facility = facility_model_;
  EXPECT_NO_THROW(new_facility->copy(facility_model_)); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(FacilityModelTests, Tick) {
  int time = 1;
  facility_model_->handleTick(time);
    //EXPECT_NO_THROW();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(FacilityModelTests, Tock) {
  int time = 1;
  EXPECT_NO_THROW(facility_model_->handleTock(time));
}
