#include <gtest/gtest.h>

#include "source_facility.h"

#include "context.h"
#include "facility_model_tests.h"
#include "model_tests.h"

using cyclus::SourceFacility;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SourceFacilityTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  SourceFacility* src_facility_;

  virtual void SetUp() {
    src_facility_ = new SourceFacility(tc_.get());
  }

  virtual void TearDown() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SourceFacilityTest, clone) {
  SourceFacility* cloned_fac =
      dynamic_cast<SourceFacility*> (src_facility_->Clone());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SourceFacilityTest, InitialState) {
  // Test things about the initial state of the facility here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SourceFacilityTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility_->str());
  // Test SourceFacility specific aspects of the print method here
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SourceFacilityTest, ReceiveMessage) {
  // Test SourceFacility specific behaviors of the ReceiveMessage function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SourceFacilityTest, Tick) {
  int time = 1;
  ASSERT_NO_THROW(src_facility_->Tick(time));
  // Test SourceFacility specific behaviors of the Tick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SourceFacilityTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility_->Tock(time));
  // Test SourceFacility specific behaviors of the Tock function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* SourceFacilityModelConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::Model*>(new SourceFacility(ctx));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::FacilityModel* SourceFacilityConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::FacilityModel*>(new SourceFacility(ctx));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(SourceFac, FacilityModelTests,
                        ::testing::Values(&SourceFacilityConstructor));

INSTANTIATE_TEST_CASE_P(SourceFac, ModelTests,
                        ::testing::Values(&SourceFacilityModelConstructor));
