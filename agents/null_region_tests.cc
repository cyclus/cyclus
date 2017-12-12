#include <gtest/gtest.h>

#include "null_region.h"

#include "agent_tests.h"
#include "region_tests.h"

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
  NullRegion* cloned_fac = dynamic_cast<NullRegion*> (src_region_->Clone());
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
  EXPECT_NO_THROW(src_region_->Tick());
  // Test NullRegion specific behaviors of the handleTick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullRegionTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_region_->Tick());
  // Test NullRegion specific behaviors of the handleTock function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* NullRegionConstructor(cyclus::Context* ctx) {
  return new NullRegion(ctx);
}

// Required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(NullRegion, RegionTests,
                        ::testing::Values(&NullRegionConstructor));
INSTANTIATE_TEST_CASE_P(NullRegion, AgentTests,
                        ::testing::Values(&NullRegionConstructor));
