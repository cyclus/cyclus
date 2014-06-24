#include <gtest/gtest.h>

#include "stub_facility.h"

#include "agent_tests.h"
#include "context.h"
#include "facility_tests.h"

using stubs::StubFacility;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubFacilityTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  StubFacility* src_facility_;

  virtual void SetUp() {
    src_facility_ = new StubFacility(tc_.get());
  }

  virtual void TearDown() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubFacilityTest, clone) {
  StubFacility* cloned_fac =
      dynamic_cast<StubFacility*> (src_facility_->Clone());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubFacilityTest, InitialState) {
  // Test things about the initial state of the facility here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubFacilityTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility_->str());
  // Test StubFacility specific aspects of the print method here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubFacilityTest, Tick) {
  ASSERT_NO_THROW(src_facility_->Tick());
  // Test StubFacility specific behaviors of the Tick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubFacilityTest, Tock) {
  EXPECT_NO_THROW(src_facility_->Tock());
  // Test StubFacility specific behaviors of the Tock function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* StubFacilityConstructor(cyclus::Context* ctx) {
  return new StubFacility(ctx);
}

// Required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(StubFac, FacilityTests,
                        ::testing::Values(&StubFacilityConstructor));

INSTANTIATE_TEST_CASE_P(StubFac, AgentTests,
                        ::testing::Values(&StubFacilityConstructor));
