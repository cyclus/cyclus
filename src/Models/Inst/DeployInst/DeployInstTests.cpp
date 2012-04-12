// DeployInstTests.cpp
#include <gtest/gtest.h>

#include "DeployInst.h"
#include "CycException.h"
#include "Message.h"
#include "InstModelTests.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeDeployInst : public DeployInst {
  public:
    FakeDeployInst() : DeployInst() {
    }

    virtual ~FakeDeployInst() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* DeployInstModelConstructor(){
  return dynamic_cast<Model*>(new FakeDeployInst());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InstModel* DeployInstConstructor(){
  return dynamic_cast<InstModel*>(new FakeDeployInst());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class DeployInstTest : public ::testing::Test {
  protected:
    FakeDeployInst* src_inst;
    FakeDeployInst* new_inst; 

    virtual void SetUp(){
      src_inst = new FakeDeployInst();
      src_inst->setParent(new TestRegion());
      new_inst = new FakeDeployInst();
      // for facilities that trade commodities, create appropriate markets here
    };

    virtual void TearDown() {
      delete src_inst;
      // for facilities that trade commodities, delete appropriate markets here
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DeployInstTest, InitialState) {
  // Test things about the initial state of the inst here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DeployInstTest, CopyFreshModel) {
  new_inst->copyFreshModel(dynamic_cast<Model*>(src_inst)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<DeployInst*>(new_inst)); // still a deploy inst
  EXPECT_NO_THROW(dynamic_cast<FakeDeployInst*>(new_inst)); // still a fake deploy inst
  // Test that DeployInst specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DeployInstTest, Print) {
  EXPECT_NO_THROW(src_inst->str());
  // Test DeployInst specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DeployInstTest, ReceiveMessage) {
  msg_ptr msg;
  // Test DeployInst specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DeployInstTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_inst->handleTick(time));
  // Test DeployInst specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DeployInstTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_inst->handleTick(time));
  // Test DeployInst specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(DeployInst, InstModelTests, Values(&DeployInstConstructor));
INSTANTIATE_TEST_CASE_P(DeployInst, ModelTests, Values(&DeployInstModelConstructor));

