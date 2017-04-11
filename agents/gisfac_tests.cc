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

  virtual void SetUp() {
    facility_a_ = new GISFac(tc_.get());
    facility_b_ = new GISFac(tc_.get());
    facility_a_->gis = new cyclus::GIS();
    facility_b_->gis = new cyclus::GIS();
  }

  virtual void TearDown() {}
};
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, set_longitude) {
  facility_a_->gis->set_longitude(46.545821);
  EXPECT_NEAR(facility_a_->gis->get_longitude(), 46.545821, 32.5 * 0.01);
  facility_a_->gis->set_longitude(-82.5546);
  EXPECT_NEAR(facility_a_->gis->get_longitude(), -82.5546, 82.5546 * 0.01);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, set_latitude) {
  facility_a_->gis->set_latitude(32.5);
  EXPECT_NEAR(facility_a_->gis->get_latitude(), 32.5, 32.5 * 0.01);
  facility_a_->gis->set_latitude(-2.5546);
  EXPECT_NEAR(facility_a_->gis->get_latitude(), -2.5546, 2.5546 * 0.01);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, InitialState) {
  // Test things about the initial state of the facility here
  EXPECT_NEAR(facility_a_->gis->get_longitude(), 0, 0.001);
  EXPECT_NEAR(facility_a_->gis->get_latitude(), 0, 0.001);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacTest, Distance) {
  EXPECT_NEAR(facility_a_->gis->Distance(*(facility_b_->gis)), 0, 0.0001);
  facility_a_->gis->set_latitude(48.858222);
  facility_a_->gis->set_longitude(2.2945);
  facility_b_->gis->set_latitude(52.37305);
  facility_b_->gis->set_longitude(4.892222);
  EXPECT_NEAR(facility_a_->gis->Distance(*(facility_b_->gis)), 432.126,
              432.126 * 0.01);
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
