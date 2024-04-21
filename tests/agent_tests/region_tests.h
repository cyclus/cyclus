#ifndef CYCLUS_TESTS_AGENT_TESTS_REGION_TESTS_H_
#define CYCLUS_TESTS_AGENT_TESTS_REGION_TESTS_H_

#include <gtest/gtest.h>

#include "agent_tests.h"
#include "context.h"
#include "region.h"
#include "platform.h"
#include "test_context.h"

using ::testing::TestWithParam;
using ::testing::Values;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Inside the test body, fixture constructor, SetUp(), and TearDown() we
// can refer to the test parameter by GetParam().  In this case, the test
// parameter is a pointer to a concrete Region instance

class RegionTests : public TestWithParam<AgentConstructor*> {
 public:
  virtual void SetUp() {
    region_ = dynamic_cast<cyclus::Region*>((*GetParam())(tc_.get()));
  }
  virtual void TearDown() {}

 protected:
  cyclus::TestContext tc_;
  cyclus::Region* region_;
};

#endif  // CYCLUS_TESTS_AGENT_TESTS_REGION_TESTS_H_
