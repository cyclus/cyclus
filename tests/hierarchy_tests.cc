#include <gtest/gtest.h>

#include "agent.h"
#include "facility.h"
#include "institution.h"
#include "region.h"
#include "test_context.h"
#include "test_agents/test_region.h"
#include "test_agents/test_inst.h"
#include "test_agents/test_facility.h"

namespace cyclus {

class HierarchyTests : public ::testing::Test {
 protected:
  virtual void SetUp() {
    // Create a proper hierarchy: Region -> Institution -> Facility
    region_ = new TestRegion(tc_.get());
    institution_ = new TestInst(tc_.get());
    facility_ = new TestFacility(tc_.get());
    
    // Build the hierarchy
    region_->Build(nullptr);
    institution_->Build(region_);
    facility_->Build(institution_);
  }

  virtual void TearDown() {
    delete facility_;
    delete institution_;
    delete region_;
  }

  TestContext tc_;
  TestRegion* region_;
  TestInst* institution_;
  TestFacility* facility_;
};

TEST_F(HierarchyTests, FacilityGetRegion) {
  // Test that a facility can find its region
  cyclus::Region* found_region = facility_->GetRegion();
  EXPECT_EQ(found_region, region_);
}

TEST_F(HierarchyTests, FacilityGetInstitution) {
  // Test that a facility can find its institution
  cyclus::Institution* found_institution = facility_->GetInstitution();
  EXPECT_EQ(found_institution, institution_);
}

TEST_F(HierarchyTests, InstitutionGetRegion) {
  // Test that an institution can find its region
  cyclus::Region* found_region = institution_->GetRegion();
  EXPECT_EQ(found_region, region_);
}

TEST_F(HierarchyTests, RegionGetRegion) {
  // Test that a region returns itself when calling GetRegion
  cyclus::Region* found_region = region_->GetRegion();
  EXPECT_EQ(found_region, region_);
}

}  // namespace cyclus
