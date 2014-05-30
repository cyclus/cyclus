#include <gtest/gtest.h>

#include <string>

#include "null_inst.h"

#include "institution_tests.h"
#include "agent_tests.h"

using cyclus::NullInst;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class NullInstTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  NullInst* src_inst_;

  virtual void SetUp() {
    src_inst_ = new NullInst(tc_.get());
  }

  virtual void TearDown() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullInstTest, clone) {
  NullInst* cloned_fac = dynamic_cast<NullInst*> (src_inst_->Clone());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullInstTest, InitialState) {
  // Test things about the initial state of the inst here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullInstTest, Print) {
  EXPECT_NO_THROW(std::string s = src_inst_->str());
  // Test NullInst specific aspects of the print method here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullInstTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_inst_->Tick());
  // Test NullInst specific behaviors of the handleTick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullInstTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_inst_->Tick());
  // Test NullInst specific behaviors of the handleTock function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* NullInstConstructor(cyclus::Context* ctx) {
  return new NullInst(ctx);
}

// required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif // CYCLUS_AGENT_TESTS_CONNECTED

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(NullInst, InstitutionTests,
                        ::testing::Values(&NullInstConstructor));
INSTANTIATE_TEST_CASE_P(NullInst, AgentTests,
                        ::testing::Values(&NullInstConstructor));
