#include <gtest/gtest.h>

#include <string>

#include "stub_inst.h"

#include "institution_tests.h"
#include "agent_tests.h"
#include "pyhooks.h"

using libname::StubInst;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubInstTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc;
  StubInst* inst;

  virtual void SetUp() {
    cyclus::PyStart();
    inst = new StubInst(tc.get());
  }

  virtual void TearDown() {
    delete inst;
    cyclus::PyStop();
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubInstTest, InitialState) {
  // Test things about the initial state of the inst here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubInstTest, Print) {
  EXPECT_NO_THROW(std::string s = inst->str());
  // Test StubInst specific aspects of the print method here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubInstTest, Tick) {
  EXPECT_NO_THROW(inst->Tick());
  // Test StubInst specific behaviors of the handleTick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubInstTest, Tock) {
  EXPECT_NO_THROW(inst->Tock());
  // Test StubInst specific behaviors of the handleTock function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Do Not Touch! Below section required for connection with Cyclus
cyclus::Agent* StubInstConstructor(cyclus::Context* ctx) {
  return new StubInst(ctx);
}
// Required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED
INSTANTIATE_TEST_CASE_P(StubInst, InstitutionTests,
                        ::testing::Values(&StubInstConstructor));
INSTANTIATE_TEST_CASE_P(StubInst, AgentTests,
                        ::testing::Values(&StubInstConstructor));
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
