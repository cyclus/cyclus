// Facilitymodel_tests.h
#include <gtest/gtest.h>

#include "facility_model.h"
#include "suffix.h"
#include "test_context.h"
#include "test_inst.h"
#include "test_market.h"

#if GTEST_HAS_PARAM_TEST

using ::testing::TestWithParam;
using ::testing::Values;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Inside the test body, fixture constructor, SetUp(), and TearDown() we
// can refer to the test parameter by GetParam().  In this case, the test
// parameter is a pointer to a concrete FacilityModel instance 
typedef cyclus::FacilityModel* FacilityModelConstructor();

class FacilityModelTests : public TestWithParam<FacilityModelConstructor*> {
 public:
  virtual void SetUp() {    
    facility_model_ = (*GetParam())();
    test_inst_ = new TestInst(tc_.get());
    facility_model_->SetParent(test_inst_);
    test_out_market_ = new TestMarket(tc_.get(), "out-commod");
    test_in_market_ = new TestMarket(tc_.get(), "in-commod");
  }
  virtual void TearDown(){ 
    delete facility_model_;
    delete test_inst_;
    delete test_out_market_;
    delete test_in_market_;
  }
    
 protected:
  cyclus::FacilityModel* facility_model_;
  TestMarket* test_out_market_;
  TestMarket* test_in_market_;
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


