#include <gtest/gtest.h>

#include <string>

#include "null_inst.h"

#include "inst_model_tests.h"
#include "model_tests.h"

using cyclus::NullInst;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class NullInstTest : public ::testing::Test {
  protected:
    cyclus::TestContext tc_;
    NullInst* src_inst_;

    virtual void SetUp(){
      src_inst_ = new NullInst(tc_.get());
    };

    virtual void TearDown() {}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(NullInstTest, clone) {
  NullInst* cloned_fac =
      dynamic_cast<NullInst*> (src_inst_->Clone());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullInstTest, InitialState) {
  // Test things about the initial state of the inst here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullInstTest, Print) {
  EXPECT_NO_THROW(std::string s = src_inst_->str());
  // Test NullInst specific aspects of the print method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullInstTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_inst_->Tick(time));
  // Test NullInst specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullInstTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_inst_->Tick(time));
  // Test NullInst specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* NullInstModelConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::Model*>(new NullInst(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::InstModel* NullInstConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::InstModel*>(new NullInst(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(NullInst, InstModelTests,
                        ::testing::Values(&NullInstConstructor));
INSTANTIATE_TEST_CASE_P(NullInst, ModelTests,
                        ::testing::Values(&NullInstModelConstructor));

