#include <gtest/gtest.h>

#include "stub_region.h"

#include "agent_tests.h"
#include "region_tests.h"
#include "pyhooks.h"

using libname::StubRegion;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubRegionTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc;
  StubRegion* region;

  virtual void SetUp() {
    cyclus::PyStart();
    region = new StubRegion(tc.get());
  }

  virtual void TearDown() {
    delete region;
    cyclus::PyStop();
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubRegionTest, InitialState) {
  // Test things about the initial state of the region here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubRegionTest, Print) {
  EXPECT_NO_THROW(std::string s = region->str());
  // Test StubRegion specific aspects of the print method here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubRegionTest, Tick) {
  EXPECT_NO_THROW(region->Tick());
  // Test StubRegion specific behaviors of the handleTick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubRegionTest, Tock) {
  EXPECT_NO_THROW(region->Tock());
  // Test StubRegion specific behaviors of the handleTock function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Do Not Touch! Below section required for connection with Cyclus
cyclus::Agent* StubRegionConstructor(cyclus::Context* ctx) {
  return new StubRegion(ctx);
}
// Required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED
INSTANTIATE_TEST_CASE_P(StubRegion, RegionTests,
                        ::testing::Values(&StubRegionConstructor));
INSTANTIATE_TEST_CASE_P(StubRegion, AgentTests,
                        ::testing::Values(&StubRegionConstructor));
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
