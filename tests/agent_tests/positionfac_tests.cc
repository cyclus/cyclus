#include <gtest/gtest.h>

#include "test_agents/test_positionfac.h"

#include <iostream>
#include "agent_tests.h"
#include "context.h"
#include "facility_tests.h"

using cyclus::TestPositionFac;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class PositionFacTest : public ::testing::Test {
 protected:
  TestPositionFac* facility_a_;
  TestPositionFac* facility_b_;

  virtual void SetUp() {
    facility_a_ = new TestPositionFac(0, 0);
    facility_b_ = new TestPositionFac(0, 0);
  }

  virtual void TearDown() {
    delete facility_a_;
    delete facility_b_;
  }
};
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(PositionFacTest, set_longitude) {
  facility_a_->longitude(46.545821);
  EXPECT_NEAR(facility_a_->longitude(), 46.545821, 32.5 * 0.01);
  facility_a_->longitude(-82.5546);
  EXPECT_NEAR(facility_a_->longitude(), -82.5546, 82.5546 * 0.01);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(PositionFacTest, set_latitude) {
  facility_a_->latitude(32.5);
  EXPECT_NEAR(facility_a_->latitude(), 32.5, 32.5 * 0.01);
  facility_a_->latitude(-2.5546);
  EXPECT_NEAR(facility_a_->latitude(), -2.5546, 2.5546 * 0.01);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(PositionFacTest, InitialState) {
  // Test things about the initial state of the facility here
  EXPECT_NEAR(facility_a_->longitude(), 0, 0.001);
  EXPECT_NEAR(facility_a_->latitude(), 0, 0.001);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(PositionFacTest, Constructor) {
  facility_a_->latitude(48.858222);
  facility_a_->longitude(2.2945);
  EXPECT_NEAR(facility_a_->latitude(), 48.858222, 48.858222 * 0.01);
  EXPECT_NEAR(facility_a_->longitude(), 2.2945, 2.2945 * 0.01);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(PositionFacTest, Distance) {
  EXPECT_NEAR(facility_a_->Distance(*(facility_b_)), 0, 0.0001);
  facility_a_->latitude(48.858222);
  facility_a_->longitude(2.2945);
  facility_b_->latitude(52.37305);
  facility_b_->longitude(4.892222);
  EXPECT_NEAR(facility_a_->Distance(*(facility_b_)), 432.126, 432.126 * 0.01);
}

// Required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED