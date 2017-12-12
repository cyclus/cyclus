#include <gtest/gtest.h>

#include "stub_facility.h"

#include "agent_tests.h"
#include "context.h"
#include "facility_tests.h"
#include "pyhooks.h"

using libname::StubFacility;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubFacilityTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc;
  StubFacility* facility;

  virtual void SetUp() {
    cyclus::PyStart();
    facility = new StubFacility(tc.get());
  }

  virtual void TearDown() {
    delete facility;
    cyclus::PyStop();
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubFacilityTest, InitialState) {
  // Test things about the initial state of the facility here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubFacilityTest, Print) {
  EXPECT_NO_THROW(std::string s = facility->str());
  // Test StubFacility specific aspects of the print method here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubFacilityTest, Tick) {
  ASSERT_NO_THROW(facility->Tick());
  // Test StubFacility specific behaviors of the Tick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubFacilityTest, Tock) {
  EXPECT_NO_THROW(facility->Tock());
  // Test StubFacility specific behaviors of the Tock function here
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Do Not Touch! Below section required for connection with Cyclus
cyclus::Agent* StubFacilityConstructor(cyclus::Context* ctx) {
  return new StubFacility(ctx);
}
// Required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED
INSTANTIATE_TEST_CASE_P(StubFacility, FacilityTests,
                        ::testing::Values(&StubFacilityConstructor));
INSTANTIATE_TEST_CASE_P(StubFacility, AgentTests,
                        ::testing::Values(&StubFacilityConstructor));
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
