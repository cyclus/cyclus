// StubRegionTests.cpp
#include <gtest/gtest.h>

#include "StubRegion.h"
#include "CycException.h"
#include "Message.h"
#include "RegionModelTests.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeStubRegion : public StubRegion {
  public:
    FakeStubRegion() : StubRegion() {
    }

    virtual ~FakeStubRegion() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubRegionTest : public ::testing::Test {
  protected:
    FakeStubRegion* src_region_;
    FakeStubRegion* new_region_; 

    virtual void SetUp(){
      src_region_ = new FakeStubRegion();
      new_region_ = new FakeStubRegion();
    };

    virtual void TearDown() {
      delete src_region_;
      delete new_region_;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* StubRegionModelConstructor(){
  return dynamic_cast<Model*>(new FakeStubRegion());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RegionModel* StubRegionConstructor(){
  return dynamic_cast<RegionModel*>(new FakeStubRegion());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubRegionTest, InitialState) {
  // Test things about the initial state of the region here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubRegionTest, CopyFreshModel) {
  new_region_->copyFreshModel(dynamic_cast<Model*>(src_region_)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<StubRegion*>(new_region_)); // still a stub region
  EXPECT_NO_THROW(dynamic_cast<FakeStubRegion*>(new_region_)); // still a fake stub region
  // Test that StubRegion specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubRegionTest, Print) {
  EXPECT_NO_THROW(std::string s = src_region_->str());
  // Test StubRegion specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubRegionTest, ReceiveMessage) {
  // Test StubRegion specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(StubRegion, RegionModelTests, Values(&StubRegionConstructor));
INSTANTIATE_TEST_CASE_P(StubRegion, ModelTests, Values(&StubRegionModelConstructor));

