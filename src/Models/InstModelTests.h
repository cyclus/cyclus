// InstModelTests.h
#include <gtest/gtest.h>

#include "InstModel.h"
#include "suffix.h"
#include "Testing/TestRegion.h"
#include "Testing/TestMarket.h"

#if GTEST_HAS_PARAM_TEST

using ::testing::TestWithParam;
using ::testing::Values;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Inside the test body, fixture constructor, SetUp(), and TearDown() we
// can refer to the test parameter by GetParam().  In this case, the test
// parameter is a pointer to a concrete InstModel instance 
typedef InstModel* InstModelConstructor();

class InstModelTests : public TestWithParam<InstModelConstructor*> {
  public:
    virtual ~InstModelTests() {
    }

    //virtual void SetUp() { 
    InstModelTests() {
      inst_model_ = (*GetParam())();
      inst_model_->setParent(new TestRegion());
      test_out_market_ = new TestMarket("out-commod");
      test_out_market_->copyFreshModel(test_out_market_);
      test_in_market_ = new TestMarket("in-commod");
      test_in_market_->copyFreshModel(test_in_market_);
    }
    virtual void TearDown(){ 
      delete inst_model_;
    }

  protected:
    InstModel* inst_model_;
    TestMarket* test_out_market_;
    TestMarket* test_in_market_;
    TestRegion* test_region_;

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


