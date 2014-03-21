#include <gtest/gtest.h>

#include "sink_facility.h"

#include "context.h"
#include "facility_agent_tests.h"
#include "agent_tests.h"

using cyclus::SimpleSink;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SimpleSinkTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  SimpleSink* src_facility_;

  virtual void SetUp() {
    src_facility_ = new SimpleSink(tc_.get());
  }

  virtual void TearDown() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SimpleSinkTest, clone) {
  SimpleSink* cloned_fac =
      dynamic_cast<SimpleSink*> (src_facility_->Clone());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SimpleSinkTest, InitialState) {
  // Test things about the initial state of the facility here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SimpleSinkTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility_->str());
  // Test SimpleSink specific aspects of the print method here
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SimpleSinkTest, ReceiveMessage) {
  // Test SimpleSink specific behaviors of the ReceiveMessage function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SimpleSinkTest, Tick) {
  int time = 1;
  ASSERT_NO_THROW(src_facility_->Tick(time));
  // Test SimpleSink specific behaviors of the Tick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SimpleSinkTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility_->Tock(time));
  // Test SimpleSink specific behaviors of the Tock function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* SimpleSinkAgentConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::Agent*>(new SimpleSink(ctx));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::FacilityAgent* SimpleSinkConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::FacilityAgent*>(new SimpleSink(ctx));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(SinkFac, FacilityAgentTests,
                        ::testing::Values(&SimpleSinkConstructor));

INSTANTIATE_TEST_CASE_P(SinkFac, AgentTests,
                        ::testing::Values(&SimpleSinkAgentConstructor));
