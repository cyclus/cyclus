#ifndef CYCLUS_TESTS_AGENT_TESTS_AGENT_TESTS_H_
#define CYCLUS_TESTS_AGENT_TESTS_AGENT_TESTS_H_

#include <gtest/gtest.h>

#include "agent.h"
#include "platform.h"
#include "test_context.h"

using ::testing::TestWithParam;
using ::testing::Values;


// Inside the test body, fixture constructor, SetUp(), and TearDown() we
// can refer to the test parameter by GetParam().  In this case, the test
// parameter is a pointer to a concrete Agent instance
typedef cyclus::Agent* AgentConstructor(cyclus::Context* ctx);

class AgentTests : public TestWithParam<AgentConstructor*> {
 public:
  virtual void SetUp() {
    agent_ = (*GetParam())(tc_.get());
  }
  virtual void TearDown() {}

 protected:
  cyclus::Agent* agent_;
  cyclus::TestContext tc_;
};

#endif  // CYCLUS_TESTS_AGENT_TESTS_AGENT_TESTS_H_
