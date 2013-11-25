#ifndef CYCLUS_TESTS_FACILITY_MODEL_TESTS_H_
#define CYCLUS_TESTS_FACILITY_MODEL_TESTS_H_

#include <gtest/gtest.h>

#include "facility_model.h"
#include "suffix.h"
#include "test_context.h"
#include "mock_inst.h"

#if GTEST_HAS_PARAM_TEST

using ::testing::TestWithParam;
using ::testing::Values;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Inside the test body, fixture constructor, SetUp(), and TearDown() we
// can refer to the test parameter by GetParam().  In this case, the test
// parameter is a pointer to a concrete FacilityModel instance 
typedef cyclus::FacilityModel* FacilityModelConstructor(cyclus::Context* ctx);

class FacilityModelTests : public TestWithParam<FacilityModelConstructor*> {
 public:
  virtual void SetUp() {    
    facility_model_ = (*GetParam())(tc_.get());
    test_inst_ = new MockInst(tc_.get());
    facility_model_->SetParent(test_inst_);
  }
  
  virtual void TearDown(){ 
    delete facility_model_;
    delete test_inst_;
  }
    
 protected:
  cyclus::FacilityModel* facility_model_;
  MockInst* test_inst_;
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

#endif // GTEST_HAS_PARAM_TEST

#endif // CYCLUS_TESTS_FACILITY_MODEL_TESTS_H_


