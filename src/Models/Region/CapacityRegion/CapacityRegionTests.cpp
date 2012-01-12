// CapacityRegionTests.cpp
#include <gtest/gtest.h>

#include "CapacityRegion.h"
#include "CycException.h"
#include "Message.h"
#include "RegionModelTests.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeCapacityRegion : public CapacityRegion {
  public:
    FakeCapacityRegion() : CapacityRegion() {
    }

    virtual ~FakeCapacityRegion() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CapacityRegionTest : public ::testing::Test {
  protected:
    FakeCapacityRegion* src_region_;
    FakeCapacityRegion* new_region_; 

    virtual void SetUp(){
      src_region_ = new FakeCapacityRegion();
      new_region_ = new FakeCapacityRegion();
    };

    virtual void TearDown() {
      delete src_region_;
      delete new_region_;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* CapacityRegionModelConstructor(){
  return dynamic_cast<Model*>(new FakeCapacityRegion());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RegionModel* CapacityRegionConstructor(){
  return dynamic_cast<RegionModel*>(new FakeCapacityRegion());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CapacityRegionTest, InitialState) {
  // Test things about the initial state of the region here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CapacityRegionTest, CopyFreshModel) {
  new_region_->copyFreshModel(dynamic_cast<Model*>(src_region_)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<CapacityRegion*>(new_region_)); // still a capacity region
  EXPECT_NO_THROW(dynamic_cast<FakeCapacityRegion*>(new_region_)); // still a fake capacity region
  // Test that CapacityRegion specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CapacityRegionTest, Print) {
  EXPECT_NO_THROW(src_region_->print());
  // Test CapacityRegion specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(CapacityRegionTest, ReceiveMessage) {
  // Test CapacityRegion specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(CapacityRegion, RegionModelTests, Values(&CapacityRegionConstructor));
INSTANTIATE_TEST_CASE_P(CapacityRegion, ModelTests, Values(&CapacityRegionModelConstructor));

