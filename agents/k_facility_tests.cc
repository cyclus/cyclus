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
  KFacility* cloned_fac = dynamic_cast<KFacility*> (src_facility_->Clone());
  delete cloned_fac;
}

cyclus::Agent* KFacilityConstructor(cyclus::Context* ctx) {
  return dynamic_cast<cyclus::Agent*>(new KFacility(ctx));
}

// required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif // CYCLUS_AGENT_TESTS_CONNECTED

INSTANTIATE_TEST_CASE_P(KFac, FacilityTests,
                        ::testing::Values(&KFacilityConstructor));

INSTANTIATE_TEST_CASE_P(KFac, AgentTests,
                        ::testing::Values(&KFacilityConstructor));
