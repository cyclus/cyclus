#include <gtest/gtest.h>

#include "source.h"

#include "context.h"
#include "facility_tests.h"
#include "agent_tests.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SourceTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  Source* src_facility_;

  virtual void SetUp() {
    src_facility_ = new Source(tc_.get());
  }

  virtual void TearDown() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SourceTest, clone) {
  Source* cloned_fac = dynamic_cast<Source*> (src_facility_->Clone());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SourceTest, InitialState) {
  // Test things about the initial state of the facility here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SourceTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility_->str());
  // Test Source specific aspects of the print method here
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SourceTest, ReceiveMessage) {
  // Test Source specific behaviors of the ReceiveMessage function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SourceTest, Tick) {
  int time = 1;
  ASSERT_NO_THROW(src_facility_->Tick());
  // Test Source specific behaviors of the Tick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SourceTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility_->Tock());
  // Test Source specific behaviors of the Tock function here
  int a = 0;
}

};  // namespace cyclus

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* SourceConstructor(cyclus::Context* ctx) {
  return new cyclus::Source(ctx);
}

// required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif // CYCLUS_AGENT_TESTS_CONNECTED

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(SourceFac, FacilityTests,
                        ::testing::Values(&SourceConstructor));

INSTANTIATE_TEST_CASE_P(SourceFac, AgentTests,
                        ::testing::Values(&SourceConstructor));
