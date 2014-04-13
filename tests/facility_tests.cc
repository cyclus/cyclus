// facility_tests.cc
#include <gtest/gtest.h>

#include "facility_tests.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(FacilityTests, DISABLED_Tick) {
  int time = 1;
  facility_->Tick(time);
    // EXPECT_NO_THROW();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(FacilityTests, DISABLED_Tock) {
  int time = 1;
  EXPECT_NO_THROW(facility_->Tock(time));
}
