#include <gtest/gtest.h>

#include "sink_facility.h"

#include "context.h"
#include "facility_model_tests.h"
#include "model_tests.h"

using cyclus::SinkFacility;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SinkFacilityTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  SinkFacility* src_facility_;

  virtual void SetUp(){
    src_facility_ = new SinkFacility(tc_.get());
  };
  
  virtual void TearDown() {}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkFacilityTest, clone) {
  SinkFacility* cloned_fac =
      dynamic_cast<SinkFacility*> (src_facility_->Clone());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SinkFacilityTest, InitialState) {
  // Test things about the initial state of the facility here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SinkFacilityTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility_->str());
  // Test SinkFacility specific aspects of the print method here
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(SinkFacilityTest, ReceiveMessage) {
  // Test SinkFacility specific behaviors of the ReceiveMessage function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkFacilityTest, Tick) {
  int time = 1;
  ASSERT_NO_THROW(src_facility_->Tick(time));
  // Test SinkFacility specific behaviors of the Tick function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SinkFacilityTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility_->Tock(time));
  // Test SinkFacility specific behaviors of the Tock function here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* SinkFacilityModelConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::Model*>(new SinkFacility(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::FacilityModel* SinkFacilityConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::FacilityModel*>(new SinkFacility(ctx));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(SinkFac, FacilityModelTests,
                        ::testing::Values(&SinkFacilityConstructor));

INSTANTIATE_TEST_CASE_P(SinkFac, ModelTests,
                        ::testing::Values(&SinkFacilityModelConstructor));
