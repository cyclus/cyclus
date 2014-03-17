#ifndef CYCLUS_TESTS_INST_MODEL_TESTS_H_
#define CYCLUS_TESTS_INST_MODEL_TESTS_H_

#include <gtest/gtest.h>

#include "institution.h"
#include "test_modules/test_facility.h"
#include "test_modules/test_inst.h"
#include "test_modules/test_region.h"
#include "suffix.h"
#include "test_context.h"
#include "agent_tests.h"

#if GTEST_HAS_PARAM_TEST

using ::testing::TestWithParam;
using ::testing::Values;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Inside the test body, fixture constructor, SetUp(), and TearDown() we
// can refer to the test parameter by GetParam().  In this case, the test
// parameter is a pointer to a concrete Institution instance 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class InstitutionTests : public TestWithParam<AgentConstructor*> {
 protected:
  TestInst* institution_;
  TestFacility* test_facility_;
  TestRegion* test_region_;
  cyclus::TestContext tc_;
  
 public:
  virtual void SetUp() { 
    institution_ = new TestInst(tc_.get());
    test_facility_ = new TestFacility(tc_.get());
    test_region_ = new TestRegion(tc_.get());
    institution_->Build(test_region_);
  }
  virtual void TearDown(){}   
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

#endif // CYCLUS_TESTS_INST_MODEL_TESTS_H_

