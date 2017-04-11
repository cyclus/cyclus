#include <gtest/gtest.h>

#include "gisfac.h"

#include <iostream>
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
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, set_longitude) {
  facility_a_->gis->set_longitude(32.5);
  EXPECT_NEAR(facility_a_->gis->get_longitude(), 32.5, 32.5 * 0.01);
  facility_a_->gis->set_longitude(-2.5546);
  EXPECT_NEAR(facility_a_->gis->get_longitude(), -2.5546, 2.5546 * 0.01);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, set_latitude) {
  facility_a_->gis->set_latitude(32.5);
  std::cout << facility_a_->gis->get_latitude() << std::endl;
  EXPECT_NEAR(facility_a_->gis->get_latitude(), 32.5, 32.5 * 0.01);
  facility_a_->gis->set_latitude(-2.5546);
  std::cout << facility_a_->gis->get_latitude() << std::endl;
  EXPECT_NEAR(facility_a_->gis->get_latitude(), -2.5546, 2.5546 * 0.01);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, InitialState) {
  // Test things about the initial state of the facility here
  EXPECT_EQ(facility_a_->gis->get_longitude(), 0);
  EXPECT_EQ(facility_a_->gis->get_latitude(), 0);
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
