// StubFacilityTests.cpp
#include <gtest/gtest.h>

#include "StubFacility.h"
#include "CycException.h"
#include "Message.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeStubFacility : public StubFacility {
  public:
    FakeStubFacility() : StubFacility() {
    }

    virtual ~FakeStubFacility() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* StubFacilityModelConstructor(){
  return dynamic_cast<Model*>(new FakeStubFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* StubFacilityConstructor(){
  return dynamic_cast<FacilityModel*>(new FakeStubFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubFacilityTest : public ::testing::Test {
  protected:
    FakeStubFacility* src_facility;
    FakeStubFacility* new_facility; 

    virtual void SetUp(){
      src_facility = new FakeStubFacility();
      src_facility->setParent(new TestInst());
      new_facility = new FakeStubFacility();
      // for facilities that trade commodities, create appropriate markets here
    };

    virtual void TearDown() {
      delete src_facility;
      delete new_facility;
      // for facilities that trade commodities, delete appropriate markets here
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubFacilityTest, InitialState) {
  // Test things about the initial state of the facility here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubFacilityTest, CopyFreshModel) {
  new_facility->copyFreshModel(dynamic_cast<Model*>(src_facility)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<StubFacility*>(new_facility)); // still a stub facility
  EXPECT_NO_THROW(dynamic_cast<FakeStubFacility*>(new_facility)); // still a fake stub facility
  // Test that StubFacility specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubFacilityTest, Print) {
  EXPECT_NO_THROW(src_facility->print());
  // Test StubFacility specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubFacilityTest, ReceiveMessage) {
  msg_ptr msg;
  // Test StubFacility specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubFacilityTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->handleTick(time));
  // Test StubFacility specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubFacilityTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->handleTick(time));
  // Test StubFacility specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(StubFac, FacilityModelTests, Values(&StubFacilityConstructor));
INSTANTIATE_TEST_CASE_P(StubFac, ModelTests, Values(&StubFacilityModelConstructor));

