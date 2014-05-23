#ifndef CYCLUS_TESTS_REGION_TESTS_H_
#define CYCLUS_TESTS_REGION_TESTS_H_

#include <gtest/gtest.h>

#include "agent_tests.h"
#include "context.h"
#include "region.h"
#include "suffix.h"
#include "test_context.h"

#if GTEST_HAS_PARAM_TEST

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

#else

// Google Test may not support value-parameterized tests with some
// compilers. If we use conditional compilation to compile out all
// code referring to the gtest_main library, MSVC linker will not link
// that library at all and consequently complain about missing entry
// point defined in that library (fatal error LNK1561: entry point
// must be defined). This dummy test keeps gtest_main linked in.
TEST(DummyTest, ValueParameterizedTestsAreNotSupportedOnThisPlatform) {}

#endif  // GTEST_HAS_PARAM_TEST

#endif  // CYCLUS_TESTS_REGION_TESTS_H_
