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

class NestedHierarchyTests : public ::testing::Test {
 protected:
  virtual void SetUp() {
    // Create a nested hierarchy: USA -> Illinois -> Metropolis -> Honeywell -> ConverDyn -> ConversionFacility
    // This simulates the example: USA (region) -> Illinois (region) -> Metropolis (region) -> 
    // Honeywell (institution) -> ConverDyn (institution) -> ConversionFacility (facility)
    
    usa_ = new TestRegion(tc_.get());
    illinois_ = new TestRegion(tc_.get());
    metropolis_ = new TestRegion(tc_.get());
    honeywell_ = new TestInst(tc_.get());
    converdyn_ = new TestInst(tc_.get());
    conversion_facility_ = new TestFacility(tc_.get());
    
    // Build the nested hierarchy
    usa_->Build(nullptr);
    illinois_->Build(usa_);
    metropolis_->Build(illinois_);
    honeywell_->Build(metropolis_);
    converdyn_->Build(honeywell_);
    conversion_facility_->Build(converdyn_);
  }

  virtual void TearDown() {
    delete conversion_facility_;
    delete converdyn_;
    delete honeywell_;
    delete metropolis_;
    delete illinois_;
    delete usa_;
  }

  TestContext tc_;
  TestRegion* usa_;
  TestRegion* illinois_;
  TestRegion* metropolis_;
  TestInst* honeywell_;
  TestInst* converdyn_;
  TestFacility* conversion_facility_;
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

TEST_F(HierarchyTests, GetAncestorOfKindLayer) {
  // Test the new layer functionality
  // Default layer (1) should return the closest ancestor
  Agent* closest_region = facility_->GetAncestorOfKind("Region");
  EXPECT_EQ(closest_region, region_);
  
  // Layer 1 should be the same as default
  Agent* layer1_region = facility_->GetAncestorOfKind("Region", 1);
  EXPECT_EQ(layer1_region, region_);
  
  // Layer -1 should return the most distant ancestor (same as closest in this case)
  Agent* last_region = facility_->GetAncestorOfKind("Region", -1);
  EXPECT_EQ(last_region, region_);
  
  // Layer 2 should return nullptr since there's only one region
  Agent* layer2_region = facility_->GetAncestorOfKind("Region", 2);
  EXPECT_EQ(layer2_region, nullptr);
}

TEST_F(HierarchyTests, GetAncestorOfKindInvalidLayer) {
  // Test invalid layer numbers
  Agent* invalid_layer = facility_->GetAncestorOfKind("Region", 0);
  EXPECT_EQ(invalid_layer, nullptr);
  
  Agent* negative_invalid = facility_->GetAncestorOfKind("Region", -2);
  EXPECT_EQ(negative_invalid, nullptr);
  
  Agent* too_large = facility_->GetAncestorOfKind("Region", 999);
  EXPECT_EQ(too_large, nullptr);
}

TEST_F(NestedHierarchyTests, NestedRegionLayers) {
  // Test the nested region scenario from the example
  // From ConversionFacility, test different region layers
  
  // Layer 1: closest region (Metropolis)
  Agent* layer1_region = conversion_facility_->GetAncestorOfKind("Region", 1);
  EXPECT_EQ(layer1_region, metropolis_);
  
  // Layer 2: second closest region (Illinois)
  Agent* layer2_region = conversion_facility_->GetAncestorOfKind("Region", 2);
  EXPECT_EQ(layer2_region, illinois_);
  
  // Layer 3: third closest region (USA)
  Agent* layer3_region = conversion_facility_->GetAncestorOfKind("Region", 3);
  EXPECT_EQ(layer3_region, usa_);
  
  // Layer -1: most distant region (USA)
  Agent* last_region = conversion_facility_->GetAncestorOfKind("Region", -1);
  EXPECT_EQ(last_region, usa_);
  
  // Layer 4: should be nullptr (no 4th region)
  Agent* layer4_region = conversion_facility_->GetAncestorOfKind("Region", 4);
  EXPECT_EQ(layer4_region, nullptr);
}

TEST_F(NestedHierarchyTests, NestedInstitutionLayers) {
  // Test nested institution layers
  
  // Layer 1: closest institution (ConverDyn)
  Agent* layer1_inst = conversion_facility_->GetAncestorOfKind("Inst", 1);
  EXPECT_EQ(layer1_inst, converdyn_);
  
  // Layer 2: second closest institution (Honeywell)
  Agent* layer2_inst = conversion_facility_->GetAncestorOfKind("Inst", 2);
  EXPECT_EQ(layer2_inst, honeywell_);
  
  // Layer -1: most distant institution (Honeywell)
  Agent* last_inst = conversion_facility_->GetAncestorOfKind("Inst", -1);
  EXPECT_EQ(last_inst, honeywell_);
  
  // Layer 3: should be nullptr (no 3rd institution)
  Agent* layer3_inst = conversion_facility_->GetAncestorOfKind("Inst", 3);
  EXPECT_EQ(layer3_inst, nullptr);
}

TEST_F(NestedHierarchyTests, DefaultLayerBehavior) {
  // Test that default layer (1) works correctly
  Agent* default_region = conversion_facility_->GetAncestorOfKind("Region");
  EXPECT_EQ(default_region, metropolis_);
  
  Agent* default_inst = conversion_facility_->GetAncestorOfKind("Inst");
  EXPECT_EQ(default_inst, converdyn_);
}

}  // namespace cyclus
