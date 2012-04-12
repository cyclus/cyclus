// StubInstTests.cpp
#include <gtest/gtest.h>

#include "StubInst.h"
#include "CycException.h"
#include "Message.h"
#include "InstModelTests.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeStubInst : public StubInst {
  public:
    FakeStubInst() : StubInst() {
    }

    virtual ~FakeStubInst() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubInstTest : public ::testing::Test {
  protected:
    FakeStubInst* src_inst;
    FakeStubInst* new_inst; 

    virtual void SetUp(){
      src_inst = new FakeStubInst();
      src_inst->setParent(new TestRegion());
      new_inst = new FakeStubInst();
      // for facilities that trade commodities, create appropriate markets here
    };

    virtual void TearDown() {
      delete src_inst;
      // for facilities that trade commodities, delete appropriate markets here
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* StubInstModelConstructor(){
  return dynamic_cast<Model*>(new FakeStubInst());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InstModel* StubInstConstructor(){
  return dynamic_cast<InstModel*>(new FakeStubInst());
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubInstTest, InitialState) {
  // Test things about the initial state of the inst here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubInstTest, CopyFreshModel) {
  new_inst->copyFreshModel(dynamic_cast<Model*>(src_inst)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<StubInst*>(new_inst)); // still a stub inst
  EXPECT_NO_THROW(dynamic_cast<FakeStubInst*>(new_inst)); // still a fake stub inst
  // Test that StubInst specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubInstTest, Print) {
  EXPECT_NO_THROW(src_inst->str());
  // Test StubInst specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubInstTest, ReceiveMessage) {
  msg_ptr msg;
  // Test StubInst specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubInstTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_inst->handleTick(time));
  // Test StubInst specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubInstTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_inst->handleTick(time));
  // Test StubInst specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(StubInst, InstModelTests, Values(&StubInstConstructor));
INSTANTIATE_TEST_CASE_P(StubInst, ModelTests, Values(&StubInstModelConstructor));

