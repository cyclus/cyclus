#include <gtest/gtest.h>

#include "null_region.h"

#include "region_tests.h"
#include "agent_tests.h"

using cyclus::NullRegion;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class NullRegionTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  NullRegion* src_region_;

  virtual void SetUp() {
    src_region_ = new NullRegion(tc_.get());
  }

  virtual void TearDown() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullRegionTest, clone) {
  NullRegion* cloned_fac =
      dynamic_cast<NullRegion*> (src_region_->Clone());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullRegionTest, InitialState) {
  // Test things about the initial state of the region here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullRegionTest, Print) {
  EXPECT_NO_THROW(std::string s = src_region_->str());
  // Test NullRegion specific aspects of the print method here
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullRegionTest, ReceiveMessage) {
  // Test NullRegion specific behaviors of the receiveMessage function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullRegionTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_region_->Tick(time));
  // Test NullRegion specific behaviors of the handleTick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullRegionTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_region_->Tick(time));
  // Test NullRegion specific behaviors of the handleTock function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* NullRegionAgentConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::Agent*>(new NullRegion(ctx));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::RegionAgent* NullRegionConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::RegionAgent*>(new NullRegion(ctx));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(NullRegion, RegionAgentTests,
                        ::testing::Values(&NullRegionConstructor));
INSTANTIATE_TEST_CASE_P(NullRegion, AgentTests,
                        ::testing::Values(&NullRegionAgentConstructor));
