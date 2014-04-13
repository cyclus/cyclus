#ifndef CYCLUS_TESTS_FACILITY_TESTS_H_
#define CYCLUS_TESTS_FACILITY_TESTS_H_

#include <gtest/gtest.h>

#include "agent_tests.h"
#include "facility.h"
#include "suffix.h"
#include "test_context.h"
#include "test_modules/test_inst.h"

#if GTEST_HAS_PARAM_TEST

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
    test_inst_ = new TestInst(tc_.get());
    facility_->Build(test_inst_);
  }

  virtual void TearDown(){}

 protected:
  cyclus::Facility* facility_;
  TestInst* test_inst_;
  cyclus::TestContext tc_;
};

#else

// Google Test may not support value-parameterized tests with some
// compilers. If we use conditional compilation to compile out all
// code referring to the gtest_main library, MSVC linker will not link
// that library at all and consequently complain about missing entry
// point defined in that library (fatal error LNK1561: entry point
// must be defined). This dummy test keeps gtest_main linked in.
TEST(DummyTest, ValueParameterizedTestsAreNotSupportedOnThisPlatform) {}

#endif  // GTEST_HAS_PARAM_TEST

#endif  // CYCLUS_TESTS_FACILITY_TESTS_H_
