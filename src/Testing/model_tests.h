// model_tests.h
#include <gtest/gtest.h>

#include "model.h"
#include "suffix.h"

#if GTEST_HAS_PARAM_TEST

using ::testing::TestWithParam;
using ::testing::Values;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Inside the test body, fixture constructor, SetUp(), and TearDown() we
// can refer to the test parameter by GetParam().  In this case, the test
// parameter is a pointer to a concrete Model instance 
typedef cyclus::Model* ModelConstructor(cyclus::Context* ctx);

class ModelTests : public TestWithParam<ModelConstructor*> {
  public:
    virtual void SetUp() { 
      ctx_ = new cyclus::Context(&ti_, &em_);
      model_ = (*GetParam())(ctx_);
    }
    virtual void TearDown(){ 
      delete model_;
      delete ctx_;
    }

  protected:
    cyclus::Context* ctx_;
    cyclus::Timer ti_;
    cyclus::EventManager em_;
    cyclus::Model* model_;

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


