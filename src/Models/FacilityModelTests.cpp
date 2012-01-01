// FacilityModelTests.cpp 
#include <gtest/gtest.h>

#include "FacilityModelTests.h"

// As a general rule, to prevent a test from affecting the tests that come
// after it, you should create and destroy the tested objects for each test
// instead of reusing them.  In this sample we will define a simple factory
// function for FacilityModel objects.  We will instantiate objects in test's
// SetUp() method and delete them in TearDown() method.

//class FacilityModelTests : public TestWithParam<CreateFacilityModelFunc*>{

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(FacilityModelTests, Print) {
  int time = 1;
  EXPECT_NO_THROW(facility_model_->print());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(FacilityModelTests, CopyFacility) {
  FacilityModel* new_facility;
  EXPECT_NO_THROW(new_facility->copy(facility_model_)); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(FacilityModelTests, Tick) {
  int time = 1;
  EXPECT_NO_THROW(facility_model_->handleTick(time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(FacilityModelTests, Tock) {
  int time = 1;
  EXPECT_NO_THROW(facility_model_->handleTock(time));
}
