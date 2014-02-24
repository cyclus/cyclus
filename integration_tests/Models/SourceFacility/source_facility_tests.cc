#include <gtest/gtest.h>

#include "source_facility.h"

#include "context.h"
#include "facility_model_tests.h"
#include "model_tests.h"

using cyclus::SimpleSource;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class SimpleSourceTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  SimpleSource* src_facility_;

  virtual void SetUp() {
    src_facility_ = new SimpleSource(tc_.get());
  }

  virtual void TearDown() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SimpleSourceTest, clone) {
  SimpleSource* cloned_fac =
      dynamic_cast<SimpleSource*> (src_facility_->Clone());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SimpleSourceTest, InitialState) {
  // Test things about the initial state of the facility here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SimpleSourceTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility_->str());
  // Test SimpleSource specific aspects of the print method here
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SimpleSourceTest, ReceiveMessage) {
  // Test SimpleSource specific behaviors of the ReceiveMessage function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SimpleSourceTest, Tick) {
  int time = 1;
  ASSERT_NO_THROW(src_facility_->Tick(time));
  // Test SimpleSource specific behaviors of the Tick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(SimpleSourceTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility_->Tock(time));
  // Test SimpleSource specific behaviors of the Tock function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Model* SimpleSourceModelConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::Model*>(new SimpleSource(ctx));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::FacilityModel* SimpleSourceConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::FacilityModel*>(new SimpleSource(ctx));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(SourceFac, FacilityModelTests,
                        ::testing::Values(&SimpleSourceConstructor));

INSTANTIATE_TEST_CASE_P(SourceFac, ModelTests,
                        ::testing::Values(&SimpleSourceModelConstructor));
