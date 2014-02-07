// Regionmodel_tests.cc 
#include <gtest/gtest.h>

#include <string>

#include "region_model_tests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(RegionModelTests, Print) {
  int time = 1;
  EXPECT_NO_THROW(std::string s = region_model_->str());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(RegionModelTests, IsRegion) {
  EXPECT_EQ("Region",region_model_->model_type());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(RegionModelTests, AllowedFacilityModels) {
  EXPECT_NO_THROW(region_model_->IsAllowedFacility(""));
}

