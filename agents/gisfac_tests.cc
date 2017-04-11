#include <gtest/gtest.h>

#include "gisfac.h"

#include "agent_tests.h"
#include "context.h"
#include "facility_tests.h"

using cyclus::GISFac;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class GISFacTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  GISFac* facility_a_;
  GISFac* facility_b_;

  virtual void SetUp() {}

  virtual void TearDown() {}
};

TEST_F(GISFacTest, set_longitude) {
  facility_a_->gis->set_longitude(92.3);
  EXPECT_EQ(92.3, facility_a_->gis->get_longitude());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, clone) {
  GISFac* cloned_fac = dynamic_cast<GISFac*>(facility_a_->Clone());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, InitialState) {
  // Test things about the initial state of the facility here
  EXPECT_EQ(facility_a_->gis->get_longitude(), 0);
  EXPECT_EQ(facility_a_->gis->get_latitude(), 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, Print) {
  EXPECT_NO_THROW(std::string s = facility_a_->str());
  // Test GISFac specific aspects of the print method here
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
