// FixedInstTests.cpp
#include <gtest/gtest.h>

#include "FixedInst.h"
#include "CycException.h"
#include "Message.h"
#include "InstModelTests.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeFixedInst : public FixedInst {
  public:
    FakeFixedInst() : FixedInst() {
    }

    virtual ~FakeFixedInst() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FixedInstTest : public ::testing::Test {
  protected:
    FakeFixedInst* src_inst;
    FakeFixedInst* new_inst; 

    virtual void SetUp(){
      src_inst = new FakeFixedInst();
      src_inst->setParent(new TestRegion());
      new_inst = new FakeFixedInst();
    };

    virtual void TearDown() {
      delete src_inst;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* FixedInstModelConstructor(){
  return dynamic_cast<Model*>(new FakeFixedInst());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InstModel* FixedInstConstructor(){
  return dynamic_cast<InstModel*>(new FakeFixedInst());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(FixedInstTest, InitialState) {
  // Test things about the initial state of the inst here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(FixedInstTest, CopyFreshModel) {
  new_inst->copyFreshModel(dynamic_cast<Model*>(src_inst)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<FixedInst*>(new_inst)); // still a fixed inst
  EXPECT_NO_THROW(dynamic_cast<FakeFixedInst*>(new_inst)); // still a fake fixed inst
  // Test that FixedInst specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(FixedInstTest, Print) {
  EXPECT_NO_THROW(src_inst->str());
  // Test FixedInst specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(FixedInstTest, ReceiveMessage) {
  msg_ptr msg;
  // Test FixedInst specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(FixedInstTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_inst->handleTick(time));
  // Test FixedInst specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(FixedInstTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_inst->handleTick(time));
  // Test FixedInst specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(FixedInst, InstModelTests, Values(&FixedInstConstructor));
INSTANTIATE_TEST_CASE_P(FixedInst, ModelTests, Values(&FixedInstModelConstructor));

