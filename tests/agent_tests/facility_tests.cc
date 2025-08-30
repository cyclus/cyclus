#include "facility_tests.h"

#include <gtest/gtest.h>

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(FacilityTests);

// The FacilityTests initialization does not honor the normal initialization
// guarantees for archetypes.  And so any type of remotely sophisticated
// configuration validation of archetypes will be triggered to fail if we try
// to call methods like e.g. Tick, Tock, Build, etc.  We already know the
// agents satisfy the Agent class interface implicitly by being able to load
// them and cast/assign them to Agent*/Facility*/etc. without compiler errors.
// From now on, only call state-reporting functions with no side-effects in
// these tests.

TEST_P(FacilityTests, Construct) {
  // make sure setup construction worked successfully
  ASSERT_NE(facility_, nullptr);
}

TEST_P(FacilityTests, Entity) {
  Json::Value a = facility_->annotations();
  EXPECT_STREQ("facility", a["entity"].asCString());
}

TEST_P(FacilityTests, GetParentRegionAndInstitution) {
  // Test that GetParentRegion and GetParentInstitution work correctly
  // These methods should return nullptr when the facility is not in a hierarchy
  cyclus::Region* region = facility_->GetParentRegion();
  cyclus::Institution* institution = facility_->GetParentInstitution();
  
  // When not in a hierarchy, both should return nullptr
  EXPECT_EQ(region, nullptr);
  EXPECT_EQ(institution, nullptr);
}
