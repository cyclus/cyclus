#include <gtest/gtest.h>

#include <string>

#include "stub_inst.h"

#include "institution_tests.h"
#include "agent_tests.h"

using stubs::StubInst;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubInstTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  StubInst* src_inst_;

  virtual void SetUp() {
    src_inst_ = new StubInst(tc_.get());
  }

  virtual void TearDown() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubInstTest, clone) {
  StubInst* cloned_fac =
      dynamic_cast<StubInst*> (src_inst_->Clone());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubInstTest, InitialState) {
  // Test things about the initial state of the inst here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubInstTest, Print) {
  EXPECT_NO_THROW(std::string s = src_inst_->str());
  // Test StubInst specific aspects of the print method here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubInstTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_inst_->Tick());
  // Test StubInst specific behaviors of the handleTick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubInstTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_inst_->Tick());
  // Test StubInst specific behaviors of the handleTock function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* StubInstitutionConstructor(cyclus::Context* ctx) {
  return new StubInst(ctx);
}

// required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif // CYCLUS_AGENT_TESTS_CONNECTED

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(StubInst, InstitutionTests,
                        ::testing::Values(&StubInstitutionConstructor));
INSTANTIATE_TEST_CASE_P(StubInst, AgentTests,
                        ::testing::Values(&StubInstitutionConstructor));
