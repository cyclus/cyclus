// BuildRegionTests.cpp
#include <gtest/gtest.h>

#include "BuildRegion.h"
#include "CycException.h"
#include "Message.h"
#include "RegionModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeBuildRegion : public BuildRegion {
  public:
    FakeBuildRegion() : BuildRegion() {
    }

    virtual ~FakeBuildRegion() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RegionModel* BuildRegionConstructor(){
  return dynamic_cast<RegionModel*>(new FakeBuildRegion());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class BuildRegionTest : public ::testing::Test {
  protected:
    FakeBuildRegion* src_region_;
    FakeBuildRegion* new_region_; 

    virtual void SetUp(){
      src_region_ = new FakeBuildRegion();
      new_region_ = new FakeBuildRegion();
    };

    virtual void TearDown() {
      delete src_region_;
      delete new_region_;
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BuildRegionTest, InitialState) {
  // Test things about the initial state of the region here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BuildRegionTest, Print) {
  EXPECT_NO_THROW(src_region_->print());
  // Test BuildRegion specific aspects of the print method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BuildRegionTest, CopyFreshModel) {
  new_region_->copyFreshModel(dynamic_cast<Model*>(src_region_)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<BuildRegion*>(new_region_)); // still a build region
  EXPECT_NO_THROW(dynamic_cast<FakeBuildRegion*>(new_region_)); // still a fake build region
  // Test that BuildRegion specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BuildRegionTest, ReceiveMessage) {
  // Test BuildRegion specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(BuildRegion, RegionModelTests, Values(&BuildRegionConstructor));

