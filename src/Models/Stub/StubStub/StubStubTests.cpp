// StubStubTests.cpp
#include <gtest/gtest.h>

#include "StubStub.h"
#include "StubModelTests.h"
#include "CycException.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeStubStub : public StubStub {
  public:
    FakeStubStub() : StubStub() {
    }

    virtual ~FakeStubStub() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubModel* StubStubConstructor(){
  return dynamic_cast<StubModel*>(new FakeStubStub());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubStubTest : public ::testing::Test {
  protected:
      FakeStubStub* src_stub_;
      FakeStubStub* new_stub_;
    
    virtual void SetUp(){
      src_stub_ = new FakeStubStub;
      new_stub_ = new FakeStubStub;
    };

    virtual void TearDown() {
      delete src_stub_;
      delete new_stub_;
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubStubTest, InitialState) {
  // Test things about the initial state of the stub here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubStubTest, CopyFreshModel) {
  new_stub_->copyFreshModel(dynamic_cast<Model*>(src_stub_)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<StubStub*>(new_stub_)); // still a build stub
  EXPECT_NO_THROW(dynamic_cast<FakeStubStub*>(new_stub_)); // still a fake build stub
  // Test that StubStub specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubStubTest, Print) {
  EXPECT_NO_THROW(src_stub_->print());
  // Test StubStub specific aspects of the print method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(StubStub, StubModelTests, Values(&StubStubConstructor));

