// Regionmodel_tests.cc 
#include <gtest/gtest.h>

#include <string>

#include "region_model_tests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(RegionAgentTests, Print) {
  int time = 1;
  EXPECT_NO_THROW(std::string s = region_model_->str());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(RegionAgentTests, IsRegion) {
  EXPECT_EQ("Region",region_model_->kind());
}

