// StubInstTests.cpp
#include <gtest/gtest.h>

#include <string>

#include "stub_inst.h"

#include "inst_model_tests.h"
#include "model_tests.h"

using stubs::StubInst;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubInstTest : public ::testing::Test {
  protected:
    cyclus::TestContext tc_;
    StubInst* src_inst_;

    virtual void SetUp(){
      src_inst_ = new StubInst(tc_.get());
    };

    virtual void TearDown() {
      delete src_inst_;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubInstTest, clone) {
  StubInst* cloned_fac =
      dynamic_cast<StubInst*> (src_inst_->Clone());
  delete cloned_fac;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubInstTest, InitialState) {
  // Test things about the initial state of the inst here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubInstTest, Print) {
  EXPECT_NO_THROW(std::string s = src_inst_->str());
  // Test StubInst specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubInstTest, ReceiveMessage) {
  cyclus::Message::Ptr msg;
  // Test StubInst specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubInstTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_inst_->HandleTick(time));
  // Test StubInst specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubInstTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_inst_->HandleTick(time));
  // Test StubInst specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* StubInstModelConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::Model*>(new StubInst(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::InstModel* StubInstConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::InstModel*>(new StubInst(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(StubInst, InstModelTests,
                        ::testing::Values(&StubInstConstructor));
INSTANTIATE_TEST_CASE_P(StubInst, ModelTests,
                        ::testing::Values(&StubInstModelConstructor));

