// institution_tests.cc
#include "institution_tests.h"

#include <gtest/gtest.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(InstitutionTests, Tick) {
  int time = 1;
  EXPECT_NO_THROW(institution_->Tick(time));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(InstitutionTests, Tock) {
  int time = 1;
  EXPECT_NO_THROW(institution_->Tock(time));
}
