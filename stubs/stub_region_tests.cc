#include <gtest/gtest.h>

#include "stub_region.h"

#include "region_model_tests.h"
#include "model_tests.h"

using stubs::StubRegion;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubRegionTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  StubRegion* src_region_;

  virtual void SetUp() {
    src_region_ = new StubRegion(tc_.get());
  }

  virtual void TearDown() {}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubRegionTest, clone) {
  StubRegion* cloned_fac =
      dynamic_cast<StubRegion*> (src_region_->Clone());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubRegionTest, InitialState) {
  // Test things about the initial state of the region here
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
TEST_F(StubRegionTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_region_->Tick(time));
  // Test StubRegion specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubRegionTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_region_->Tick(time));
  // Test StubRegion specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* StubRegionModelConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::Model*>(new StubRegion(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::RegionModel* StubRegionConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::RegionModel*>(new StubRegion(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(StubRegion, RegionModelTests,
                        ::testing::Values(&StubRegionConstructor));
INSTANTIATE_TEST_CASE_P(StubRegion, ModelTests,
                        ::testing::Values(&StubRegionModelConstructor));
