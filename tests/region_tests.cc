// Regionagent_tests.cc
#include "region_tests.h"

#include <string>

#include <gtest/gtest.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(RegionTests, Print) {
  int time = 1;
  EXPECT_NO_THROW(std::string s = region_->str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_P(RegionTests, IsRegion) {
  EXPECT_EQ("Region", region_->kind());
}
