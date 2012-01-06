// InstModelTests.h
#include <gtest/gtest.h>

#include "InstModel.h"
#include "suffix.h"
#include "Testing/TestRegion.h"

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
    virtual void SetUp() { 
      inst_model_ = (*GetParam())();
      test_region_ = new TestRegion();
      inst_model_->setParent(test_region_);
    }
    virtual void TearDown(){ 
      delete inst_model_;
      delete test_region_;
    }

  protected:
    InstModel* inst_model_;
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


