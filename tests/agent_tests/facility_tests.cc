#include "facility_tests.h"

#include <gtest/gtest.h>

// extern int ConnectFacTests() {return 0;}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(FacilityTests, Tick) {
  int time = 1;
  facility_->Tick();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(FacilityTests, Tock) {
  int time = 1;
  EXPECT_NO_THROW(facility_->Tock());
}
