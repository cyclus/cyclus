#include "institution_tests.h"

#include <gtest/gtest.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(InstitutionTests, Tick) {
  int time = 1;
  EXPECT_NO_THROW(institution_->Tick());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(InstitutionTests, Tock) {
  int time = 1;
  EXPECT_NO_THROW(institution_->Tock());
}

TEST_P(InstitutionTests, Entity) {
  Json::Value a = institution_->annotations();
  EXPECT_STREQ("institution", a["entity"].asCString());
}
