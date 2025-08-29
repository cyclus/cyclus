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
    // Create a hierarchy with sub-facilities: 
    // USA -> Illinois -> Metropolis -> Honeywell -> ConverDyn -> ConversionFacility -> StorageFacility
    // This simulates: Region -> Region -> Region -> Institution -> Institution -> Facility -> SubFacility
    
    usa_ = new TestRegion(tc_.get());
    illinois_ = new TestRegion(tc_.get());
    metropolis_ = new TestRegion(tc_.get());
    honeywell_ = new TestInst(tc_.get());
    converdyn_ = new TestInst(tc_.get());
    conversion_facility_ = new TestFacility(tc_.get());
    storage_facility_ = new TestFacility(tc_.get());
    
    // Build the hierarchy with sub-facilities
    usa_->Build(nullptr);
    illinois_->Build(usa_);
    metropolis_->Build(illinois_);
    honeywell_->Build(metropolis_);
    converdyn_->Build(honeywell_);
    conversion_facility_->Build(converdyn_);
    storage_facility_->Build(conversion_facility_);
  }

  virtual void TearDown() {
    delete storage_facility_;
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
  TestFacility* storage_facility_;
};

TEST_F(HierarchyTests, BasicHierarchyTraversal) {
  // Test basic hierarchy traversal for different entity types
  
  // Test that a facility can find its region
  cyclus::Region* found_region = facility_->GetRegion();
  EXPECT_EQ(found_region, region_);
  
  // Test that a facility can find its institution
  cyclus::Institution* found_institution = facility_->GetInstitution();
  EXPECT_EQ(found_institution, institution_);
  
  // Test that an institution can find its region
  cyclus::Region* inst_region = institution_->GetRegion();
  EXPECT_EQ(inst_region, region_);
}

TEST_F(HierarchyTests, LayerFunctionality) {
  // Test layer functionality including valid and invalid layer numbers
  
  // Test valid layer functionality
  // Default layer (1) should return the closest ancestor
  Region* closest_region = facility_->GetRegion();
  EXPECT_EQ(closest_region, region_);
  
  // Layer 1 should be the same as default
  Region* layer1_region = facility_->GetRegion(1);
  EXPECT_EQ(layer1_region, region_);
  
  // Layer -1 should return the most distant ancestor (same as closest in this case)
  Region* last_region = facility_->GetRegion(-1);
  EXPECT_EQ(last_region, region_);
  
  // Layer 2 should return nullptr since there's only one region
  Region* layer2_region = facility_->GetRegion(2);
  EXPECT_EQ(layer2_region, nullptr);
  
  // Test invalid layer numbers
  Region* invalid_layer = facility_->GetRegion(0);
  EXPECT_EQ(invalid_layer, nullptr);
  
  Region* negative_invalid = facility_->GetRegion(-2);
  EXPECT_EQ(negative_invalid, nullptr);
  
  Region* too_large = facility_->GetRegion(999);
  EXPECT_EQ(too_large, nullptr);
}

TEST_F(NestedHierarchyTests, NestedLayerAccess) {
  // Test nested layer access for different entity types in complex hierarchy
  
  // Test nested region layers from StorageFacility
  // Layer 1: closest region (Metropolis)
  Region* layer1_region = storage_facility_->GetRegion(1);
  EXPECT_EQ(layer1_region, metropolis_);
  
  // Layer 2: second closest region (Illinois)
  Region* layer2_region = storage_facility_->GetRegion(2);
  EXPECT_EQ(layer2_region, illinois_);
  
  // Layer 3: third closest region (USA)
  Region* layer3_region = storage_facility_->GetRegion(3);
  EXPECT_EQ(layer3_region, usa_);
  
  // Layer -1: most distant region (USA)
  Region* last_region = storage_facility_->GetRegion(-1);
  EXPECT_EQ(last_region, usa_);
  
  // Layer 4: should be nullptr (no 4th region)
  Region* layer4_region = storage_facility_->GetRegion(4);
  EXPECT_EQ(layer4_region, nullptr);
  
  // Test nested institution layers
  // Layer 1: closest institution (ConverDyn)
  Institution* layer1_inst = storage_facility_->GetInstitution(1);
  EXPECT_EQ(layer1_inst, converdyn_);
  
  // Layer 2: second closest institution (Honeywell)
  Institution* layer2_inst = storage_facility_->GetInstitution(2);
  EXPECT_EQ(layer2_inst, honeywell_);
  
  // Layer -1: most distant institution (Honeywell)
  Institution* last_inst = storage_facility_->GetInstitution(-1);
  EXPECT_EQ(last_inst, honeywell_);
  
  // Layer 3: should be nullptr (no 3rd institution)
  Institution* layer3_inst = storage_facility_->GetInstitution(3);
  EXPECT_EQ(layer3_inst, nullptr);
  
  // Test that default layer (1) works correctly
  Region* default_region = storage_facility_->GetRegion();
  EXPECT_EQ(default_region, metropolis_);
  
  Institution* default_inst = storage_facility_->GetInstitution();
  EXPECT_EQ(default_inst, converdyn_);
}

TEST_F(NestedHierarchyTests, FacilityAPI) {
  // Test facility and region layer API functionality
  
  // Test facility layer API
  // Default (layer 1) should return closest region/institution
  Region* default_region = storage_facility_->GetRegion();
  EXPECT_EQ(default_region, metropolis_);
  
  Institution* default_inst = storage_facility_->GetInstitution();
  EXPECT_EQ(default_inst, converdyn_);
  
  // Layer 2 should return second closest
  Region* layer2_region = storage_facility_->GetRegion(2);
  EXPECT_EQ(layer2_region, illinois_);
  
  Institution* layer2_inst = storage_facility_->GetInstitution(2);
  EXPECT_EQ(layer2_inst, honeywell_);
  
  // Layer 3 should return third closest
  Region* layer3_region = storage_facility_->GetRegion(3);
  EXPECT_EQ(layer3_region, usa_);
  
  // Layer -1 should return most distant
  Region* last_region = storage_facility_->GetRegion(-1);
  EXPECT_EQ(last_region, usa_);
  
  Institution* last_inst = storage_facility_->GetInstitution(-1);
  EXPECT_EQ(last_inst, honeywell_);
  
  // Test region-to-region relationships
  // Metropolis should find Illinois as its parent region (layer 1)
  Region* metropolis_parent = metropolis_->GetRegion(1);
  EXPECT_EQ(metropolis_parent, illinois_);
  
  // Illinois should find USA as its parent region (layer 1)
  Region* illinois_parent = illinois_->GetRegion(1);
  EXPECT_EQ(illinois_parent, usa_);
  
  // USA should find nullptr (no parent region)
  Region* usa_parent = usa_->GetRegion(1);
  EXPECT_EQ(usa_parent, nullptr);
  
  // Test layer functionality for regions
  // From Metropolis, layer 2 should find USA
  Region* metropolis_grandparent = metropolis_->GetRegion(2);
  EXPECT_EQ(metropolis_grandparent, usa_);
  
  // From Metropolis, layer -1 should find USA (most distant)
  Region* metropolis_root = metropolis_->GetRegion(-1);
  EXPECT_EQ(metropolis_root, usa_);
}

TEST_F(NestedHierarchyTests, GetAllAPI) {
  // Test the GetAllParent* methods from various perspectives in the hierarchy
  
  // Test from storage facility (deepest level)
  std::vector<Region*> all_regions = storage_facility_->GetAllParentRegions();
  EXPECT_EQ(all_regions.size(), 3);
  EXPECT_EQ(all_regions[0], metropolis_);   // closest
  EXPECT_EQ(all_regions[1], illinois_);     // second closest
  EXPECT_EQ(all_regions[2], usa_);          // farthest
  
  std::vector<Institution*> all_institutions = storage_facility_->GetAllParentInstitutions();
  EXPECT_EQ(all_institutions.size(), 2);
  EXPECT_EQ(all_institutions[0], converdyn_);  // closest
  EXPECT_EQ(all_institutions[1], honeywell_);  // farthest
  
  std::vector<Facility*> all_facilities = storage_facility_->GetAllParentFacilities();
  EXPECT_EQ(all_facilities.size(), 1);
  EXPECT_EQ(all_facilities[0], conversion_facility_);  // closest parent facility
  
  // Test from institution perspective
  std::vector<Region*> inst_regions = converdyn_->GetAllParentRegions();
  EXPECT_EQ(inst_regions.size(), 3);
  EXPECT_EQ(inst_regions[0], metropolis_);
  EXPECT_EQ(inst_regions[1], illinois_);
  EXPECT_EQ(inst_regions[2], usa_);
  
  // Test institution's GetAllParentInstitutions
  std::vector<Institution*> inst_institutions = converdyn_->GetAllParentInstitutions();
  EXPECT_EQ(inst_institutions.size(), 1);
  EXPECT_EQ(inst_institutions[0], honeywell_);
  
  // Test from region perspective
  std::vector<Region*> metropolis_regions = metropolis_->GetAllParentRegions();
  EXPECT_EQ(metropolis_regions.size(), 2);
  EXPECT_EQ(metropolis_regions[0], illinois_);
  EXPECT_EQ(metropolis_regions[1], usa_);
  
  std::vector<Region*> illinois_regions = illinois_->GetAllParentRegions();
  EXPECT_EQ(illinois_regions.size(), 1);
  EXPECT_EQ(illinois_regions[0], usa_);
  
  std::vector<Region*> usa_regions = usa_->GetAllParentRegions();
  EXPECT_EQ(usa_regions.size(), 0);  // USA has no parent regions
  
  // Test from conversion facility perspective
  std::vector<Facility*> conversion_parent_facilities = conversion_facility_->GetAllParentFacilities();
  EXPECT_EQ(conversion_parent_facilities.size(), 0);  // Conversion facility has no parent facilities
}

}  // namespace cyclus
