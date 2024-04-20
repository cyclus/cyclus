#ifndef CYCLUS_TESTS_AGENT_TESTS_FACILITY_TESTS_H_
#define CYCLUS_TESTS_AGENT_TESTS_FACILITY_TESTS_H_

#include <gtest/gtest.h>

#include "agent_tests.h"
#include "facility.h"
#include "platform.h"
#include "test_context.h"
#include "test_agents/test_inst.h"

using ::testing::TestWithParam;
using ::testing::Values;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Inside the test body, fixture constructor, SetUp(), and TearDown() we
// can refer to the test parameter by GetParam().  In this case, the test
// parameter is a pointer to a concrete Facility instance

class FacilityTests : public TestWithParam<AgentConstructor*> {
 public:
  virtual void SetUp() {
    facility_ = dynamic_cast<cyclus::Facility*>((*GetParam())(tc_.get()));
  }

  virtual void TearDown() {}

 protected:
  cyclus::Facility* facility_;
  cyclus::TestContext tc_;
};

#endif  // CYCLUS_TESTS_AGENT_TESTS_FACILITY_TESTS_H_
