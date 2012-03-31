// NullFacilityTests.cpp
#include <gtest/gtest.h>

#include "NullFacility.h"
#include "CycException.h"
#include "Message.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"

#include <string>
#include <queue>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeNullFacility : public NullFacility {
  public:
    FakeNullFacility() : NullFacility() {
      in_commod_ = "in-commod";
      out_commod_ = "out-commod";
      inventory_.setCapacity(10);
      stocks_.setCapacity(10);
      capacity_ = 2;
    }

    string getOutCommod(){ return out_commod_;}
    string getInCommod(){ return in_commod_;}

    virtual ~FakeNullFacility() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* NullModelConstructor(){
  return dynamic_cast<Model*>(new FakeNullFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* NullFacilityConstructor(){
  return dynamic_cast<FacilityModel*>(new FakeNullFacility());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class NullFacilityTest : public ::testing::Test {
  protected:
    FakeNullFacility* src_facility;
    FakeNullFacility* new_facility; 
    TestMarket* out_market_;
    TestMarket* in_market_;

    virtual void SetUp(){
      src_facility = new FakeNullFacility();
      src_facility->setParent(new TestInst());
      new_facility = new FakeNullFacility();
      out_market_ = new TestMarket(src_facility->getOutCommod());
      in_market_ = new TestMarket(src_facility->getInCommod());
      // for facilities that trade commodities, create appropriate markets here
    };

    virtual void TearDown() {
      delete src_facility;
      delete new_facility;
      delete in_market_;
      delete out_market_;
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
  msg_ptr msg;
  // Test NullFacility specific behaviors of the receiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(NullFacilityTest, Tick) {
  int time = 1;
  EXPECT_NO_THROW();
  src_facility->handleTick(time);
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
INSTANTIATE_TEST_CASE_P(NullFac, ModelTests, Values(&NullModelConstructor));

