#include <gtest/gtest.h>

#include "k_facility.h"

#include "context.h"
#include "facility_tests.h"
#include "agent_tests.h"

using cyclus::KFacility;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class KFacilityTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc_;
  KFacility* src_facility_;

  virtual void SetUp() {
    src_facility_ = new KFacility(tc_.get());
  }

  virtual void TearDown() {}
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(KFacilityTest, clone) {
  KFacility* cloned_fac =
      dynamic_cast<KFacility*> (src_facility_->Clone());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(KFacilityTest, InitialState) {
  // Test things about the initial state of the facility here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(KFacilityTest, Print) {
  EXPECT_NO_THROW(std::string s = src_facility_->str());
  // Test KFacility specific aspects of the print method here
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(KFacilityTest, ReceiveMessage) {
  // Test KFacility specific behaviors of the ReceiveMessage function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(KFacilityTest, Tick) {
  int time = 1;
  ASSERT_NO_THROW(src_facility_->Tick(time));
  // Test KFacility specific behaviors of the Tick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(KFacilityTest, Tock) {
  int time = 1;
  EXPECT_NO_THROW(src_facility_->Tock(time));
  // Test KFacility specific behaviors of the Tock function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::Agent* KFacilityConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::Agent*>(new KFacility(ctx));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANTIATE_TEST_CASE_P(KFac, FacilityTests,
                        ::testing::Values(&KFacilityConstructor));

INSTANTIATE_TEST_CASE_P(KFac, AgentTests,
                        ::testing::Values(&KFacilityConstructor));
