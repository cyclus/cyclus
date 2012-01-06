// SeparationsMatrixFacilityTests.cpp
#include <gtest/gtest.h>

#include "SeparationsMatrixFacility.h"
#include "CycException.h"
#include "Message.h"
#include "FacilityModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeSeparationsMatrixFacility : public SeparationsMatrixFacility {
  public:
    FakeSeparationsMatrixFacility() : SeparationsMatrixFacility() {
    }

    virtual ~FakeSeparationsMatrixFacility() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* SeparationsMatrixFacilityConstructor(){
  return dynamic_cast<FacilityModel*>(new FakeSeparationsMatrixFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SeparationsMatrixFacilityTest : public ::testing::Test {
  protected:
    FakeSeparationsMatrixFacility* src_facility;
    FakeSeparationsMatrixFacility* new_facility; 

    virtual void SetUp(){
      src_facility = new FakeSeparationsMatrixFacility();
      src_facility->setParent(new TestInst());
      new_facility = new FakeSeparationsMatrixFacility();
      // for facilities that trade commodities, create appropriate markets here
    };

    virtual void TearDown() {
      delete src_facility;
      delete new_facility;
      // for facilities that trade commodities, delete appropriate markets here
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SeparationsMatrixFacilityTest, InitialState) {
  // Test things about the initial state of the facility here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SeparationsMatrixFacilityTest, CopyFreshModel) {
  new_facility->copyFreshModel(dynamic_cast<Model*>(src_facility)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<SeparationsMatrixFacility*>(new_facility)); // still a separations facility
  EXPECT_NO_THROW(dynamic_cast<FakeSeparationsMatrixFacility*>(new_facility)); // still a fake separations facility
  // Test that SeparationsMatrixFacility specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SeparationsMatrixFacilityTest, Print) {
  EXPECT_NO_THROW(src_facility->print());
  // Test SeparationsMatrixFacility specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SeparationsMatrixFacilityTest, ReceiveMessage) {
  Message* msg;
  // Test SeparationsMatrixFacility specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SeparationsMatrixFacilityTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->handleTick(time));
  // Test SeparationsMatrixFacility specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SeparationsMatrixFacilityTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->handleTick(time));
  // Test SeparationsMatrixFacility specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(SeparationsMatrixFac, FacilityModelTests, Values(&SeparationsMatrixFacilityConstructor));

