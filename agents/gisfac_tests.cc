#include <gtest/gtest.h>

#include "gisfac.h"

#include "context.h"
#include "facility_tests.h"
#include "agent_tests.h"

using cyclus::GISFac;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class GISFacTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  GISFac* src_facility_;

  virtual void SetUp() {
    src_facility_ = new GISFac(tc_.get());
  }

  virtual void TearDown() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, clone) {
  GISFac* cloned_fac = dynamic_cast<Sink*> (src_facility_->Clone());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, InitialState) {
  // Test things about the initial state of the facility here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility_->str());
  // Test GISFac specific aspects of the print method here
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, ReceiveMessage) {
  // Test GISFac specific behaviors of the ReceiveMessage function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, Tick) {
  int time = 1;
  ASSERT_NO_THROW(src_facility_->Tick());
  // Test GISFac specific behaviors of the Tick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility_->Tock());
  // Test GISFac specific behaviors of the Tock function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* GISFacConstructor(cyclus::Context* ctx) {
  return new GISFac(ctx);
}

// Required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(GISFacFac, FacilityTests,
                        ::testing::Values(&GISFacConstructor));

INSTANTIATE_TEST_CASE_P(GISFacFac, AgentTests,
                        ::testing::Values(&GISFacConstructor));
