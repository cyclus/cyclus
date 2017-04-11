#include <gtest/gtest.h>

#include "gisfacisa.h"

#include <iostream>
#include "agent_tests.h"
#include "context.h"
#include "facility_tests.h"

using cyclus::GISFacisa;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class GISFacisaTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  GISFacisa* facility_a_;
  GISFacisa* facility_b_;

  virtual void SetUp() {
    facility_a_ = new GISFacisa(tc_.get());
    facility_b_ = new GISFacisa(tc_.get());
    facility_a_->gis = new cyclus::GIS();
    facility_b_->gis = new cyclus::GIS();
  }

  virtual void TearDown() {
    delete facility_a_->gis;
    delete facility_b_->gis;
    delete facility_a_;
    delete facility_b_;
  }
};
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacisaTest, set_longitude) {
  facility_a_->gis->set_longitude(46.545821);
  EXPECT_NEAR(facility_a_->gis->get_longitude(), 46.545821, 32.5 * 0.01);
  facility_a_->gis->set_longitude(-82.5546);
  EXPECT_NEAR(facility_a_->gis->get_longitude(), -82.5546, 82.5546 * 0.01);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacisaTest, set_latitude) {
  facility_a_->gis->set_latitude(32.5);
  EXPECT_NEAR(facility_a_->gis->get_latitude(), 32.5, 32.5 * 0.01);
  facility_a_->gis->set_latitude(-2.5546);
  EXPECT_NEAR(facility_a_->gis->get_latitude(), -2.5546, 2.5546 * 0.01);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacisaTest, InitialState) {
  // Test things about the initial state of the facility here
  EXPECT_NEAR(facility_a_->gis->get_longitude(), 0, 0.001);
  EXPECT_NEAR(facility_a_->gis->get_latitude(), 0, 0.001);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacisaTest, Constructor) {
  delete facility_a_->gis;
  facility_a_->gis = new cyclus::GIS(48.858222, 2.2945);
  EXPECT_NEAR(facility_a_->gis->get_latitude(), 48.858222, 48.858222 * 0.01);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(GISFacisaTest, Distance) {
  EXPECT_NEAR(facility_a_->gis->Distance(*(facility_b_->gis)), 0, 0.0001);
  facility_a_->gis->set_latitude(48.858222);
  facility_a_->gis->set_longitude(2.2945);
  facility_b_->gis->set_latitude(52.37305);
  facility_b_->gis->set_longitude(4.892222);
  EXPECT_NEAR(facility_a_->gis->Distance(*(facility_b_->gis)), 432.126,
              432.126 * 0.01);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* GISFacisaConstructor(cyclus::Context* ctx) {
  return new GISFacisa(ctx);
}

// Required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(GISFacisaFac, FacilityTests,
                        ::testing::Values(&GISFacisaConstructor));

INSTANTIATE_TEST_CASE_P(GISFacisaFac, AgentTests,
                        ::testing::Values(&GISFacisaConstructor));
