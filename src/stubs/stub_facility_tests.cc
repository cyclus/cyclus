// StubFacilityTests.cpp
#include <gtest/gtest.h>

#include "stub_facility.h"

#include "context.h"
#include "facility_model_tests.h"
#include "model_tests.h"

using stub::StubFacility;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubFacilityTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  StubFacility* src_facility;

  virtual void SetUp(){
    src_facility = new StubFacility(tc_.get());
    // for facilities that trade commodities, create appropriate markets here
  };
  
  virtual void TearDown() {
    delete src_facility;
    // for facilities that trade commodities, delete appropriate markets here
  }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubFacilityTest, clone) {
  StubFacility* cloned_fac =
      dynamic_cast<StubFacility*> (src_facility->Clone());
  delete cloned_fac;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubFacilityTest, InitialState) {
  // Test things about the initial state of the facility here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubFacilityTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility->str());
  // Test StubFacility specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(StubFacilityTest, ReceiveMessage) {
  // Test StubFacility specific behaviors of the ReceiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubFacilityTest, Tick) {
  int time = 1;
  ASSERT_NO_THROW(src_facility->HandleTick(time));
  // Test StubFacility specific behaviors of the HandleTick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(StubFacilityTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility->HandleTock(time));
  // Test StubFacility specific behaviors of the HandleTock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* StubFacilityModelConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::Model*>(new StubFacility(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::FacilityModel* StubFacilityConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::FacilityModel*>(new StubFacility(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(StubFac, FacilityModelTests,
                        ::testing::Values(&StubFacilityConstructor));

INSTANTIATE_TEST_CASE_P(StubFac, ModelTests,
                        ::testing::Values(&StubFacilityModelConstructor));
