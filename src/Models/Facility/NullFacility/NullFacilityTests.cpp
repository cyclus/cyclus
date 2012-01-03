// NullFacilityTests.cpp
#include <gtest/gtest.h>

#include "NullFacility.h"
#include "CycException.h"
#include "Message.h"
#include "FacilityModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeNullFacility : public NullFacility {
  public:
    FakeNullFacility() : NullFacility() {
      in_commod_ = "in-commod";
      out_commod_ = "out-commod";
      inventory_size_ = 10;
      capacity_ = 2;

      inventory_ = deque<Material*>();
      stocks_ = deque<Material*>();
      ordersWaiting_ = deque<Message*>();
    }

    virtual ~FakeNullFacility() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* NullFacilityConstructor(){
  return dynamic_cast<FacilityModel*>(new FakeNullFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class NullFacilityTest : public ::testing::Test {
  protected:
    FakeNullFacility* src_facility;
    FakeNullFacility* new_facility; 

    virtual void SetUp(){
      src_facility = new FakeNullFacility();
      src_facility->setParent(new TestInst());
      new_facility = new FakeNullFacility();
      // for facilities that trade commodities, create appropriate markets here
    };

    virtual void TearDown() {
      delete src_facility;
      // for facilities that trade commodities, delete appropriate markets here
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullFacilityTest, InitialState) {
  // Test things about the initial state of the facility here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullFacilityTest, CopyFreshModel) {
  new_facility->copyFreshModel(dynamic_cast<Model*>(src_facility)); // deep copy
  EXPECT_NO_THROW(dynamic_cast<NullFacility*>(new_facility)); // still a null facility
  EXPECT_NO_THROW(dynamic_cast<FakeNullFacility*>(new_facility)); // still a fake null facility
  // Test that NullFacility specific parameters are initialized in the deep copy method here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullFacilityTest, Print) {
  EXPECT_NO_THROW(src_facility->print());
  // Test NullFacility specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullFacilityTest, ReceiveMessage) {
  Message* msg;
  // Test NullFacility specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullFacilityTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->handleTick(time));
  // Test NullFacility specific behaviors of the handleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullFacilityTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->handleTick(time));
  // Test NullFacility specific behaviors of the handleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(NullFac, FacilityModelTests, Values(&NullFacilityConstructor));

