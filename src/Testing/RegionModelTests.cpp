// RegionModelTests.cpp 
#include <gtest/gtest.h>

#include "RegionModelTests.h"
#include "TestFacility.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(RegionModelTests, Print) {
  int time = 1;
  EXPECT_NO_THROW(std::string s = region_model_->str());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(RegionModelTests, IsRegion) {
  EXPECT_EQ("Region",region_model_->modelType());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(RegionModelTests, AllowedFacilityModels) {
  TestFacility* test_fac = new TestFacility(); 
  EXPECT_NO_THROW(region_model_->isAllowedFacility(test_fac));
}

