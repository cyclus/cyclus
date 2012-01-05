// RegionModelTests.cpp 
#include <gtest/gtest.h>

#include "RegionModelTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(RegionModelTests, Print) {
  int time = 1;
  EXPECT_NO_THROW(region_model_->print());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(RegionModelTests, CopyInst) {
  RegionModel* new_region = region_model_;
  EXPECT_NO_THROW(new_region->copy(region_model_)); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(RegionModelTests, Tick) {
  int time = 1;
  EXPECT_NO_THROW(region_model_->handleTick(time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(RegionModelTests, Tock) {
  int time = 1;
  EXPECT_NO_THROW(region_model_->handleTock(time));
}
