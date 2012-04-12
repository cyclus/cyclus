// StubStubCommTests.cpp
#include <gtest/gtest.h>

#include "StubStubComm.h"
#include "StubCommModelTests.h"
#include "CycException.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeStubStubComm : public StubStubComm {
  public:
    FakeStubStubComm() : StubStubComm() {
    }

    virtual ~FakeStubStubComm() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubStubCommTest : public ::testing::Test {
  protected:
      FakeStubStubComm* src_stub_;
      FakeStubStubComm* new_stub_;
    
    virtual void SetUp(){
      src_stub_ = new FakeStubStubComm;
      new_stub_ = new FakeStubStubComm;
    };

    virtual void TearDown() {
      delete src_stub_;
      delete new_stub_;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubCommModel* StubStubCommModelConstructor(){
  return dynamic_cast<Model*>(new FakeStubStubComm());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubCommModel* StubStubCommConstructor(){
  return dynamic_cast<StubCommModel*>(new FakeStubStubComm());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubStubCommTest, InitialState) {
  // Test things about the initial state of the stub here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubStubCommTest, CopyFreshModel) {
  new_stub_->copyFreshModel(dynamic_cast<Model*>(src_stub_)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<StubStubComm*>(new_stub_)); // still a build stub
  EXPECT_NO_THROW(dynamic_cast<FakeStubStubComm*>(new_stub_)); // still a fake build stub
  // Test that StubStubComm specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubStubCommTest, Print) {
  EXPECT_NO_THROW(std::string s = src_stub_->str());
  // Test StubStubComm specific aspects of the print method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(StubStubComm, StubCommModelTests, Values(&StubStubCommConstructor));
INSTANTIATE_TEST_CASE_P(StubStubComm, ModelTests, Values(&StubStubCommModelConstructor));

