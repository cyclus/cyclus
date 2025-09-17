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
  
  // Test that a facility can find its parent region
  cyclus::Region* found_region = facility_->GetParentRegion();
  EXPECT_EQ(found_region, region_);
  
  // Test that a facility can find its parent institution
  cyclus::Institution* found_institution = facility_->GetParentInstitution();
  EXPECT_EQ(found_institution, institution_);
  
  // Test that an institution can find its parent region
  cyclus::Region* inst_region = institution_->GetParentRegion();
  EXPECT_EQ(inst_region, region_);
}

TEST_F(NestedHierarchyTests, NestedLayerAccess) {
  // Test nested layer access for different entity types in complex hierarchy
  
  // Test nested region layers from StorageFacility
  // Layer 1: closest region (Metropolis)
  Region* layer1_region = storage_facility_->GetParentRegion(1);
  EXPECT_EQ(layer1_region, metropolis_);
  
  // Layer 2: second closest region (Illinois)
  Region* layer2_region = storage_facility_->GetParentRegion(2);
  EXPECT_EQ(layer2_region, illinois_);
  
  // Layer 3: third closest region (USA)
  Region* layer3_region = storage_facility_->GetParentRegion(3);
  EXPECT_EQ(layer3_region, usa_);
  
  // Layer -1: most distant region (USA)
  Region* last_region = storage_facility_->GetParentRegion(-1);
  EXPECT_EQ(last_region, usa_);
  
  // Layer 4: should be nullptr (no 4th region)
  Region* layer4_region = storage_facility_->GetParentRegion(4);
  EXPECT_EQ(layer4_region, nullptr);
  
  // Test nested institution layers
  // Layer 1: closest institution (ConverDyn)
  Institution* layer1_inst = storage_facility_->GetParentInstitution(1);
  EXPECT_EQ(layer1_inst, converdyn_);
  
  // Layer 2: second closest institution (Honeywell)
  Institution* layer2_inst = storage_facility_->GetParentInstitution(2);
  EXPECT_EQ(layer2_inst, honeywell_);
  
  // Layer -1: most distant institution (Honeywell)
  Institution* last_inst = storage_facility_->GetParentInstitution(-1);
  EXPECT_EQ(last_inst, honeywell_);
  
  // Layer 3: should be nullptr (no 3rd institution)
  Institution* layer3_inst = storage_facility_->GetParentInstitution(3);
  EXPECT_EQ(layer3_inst, nullptr);
  
  // Test that default layer (1) works correctly
  Region* default_region = storage_facility_->GetParentRegion();
  EXPECT_EQ(default_region, metropolis_);
  
  Institution* default_inst = storage_facility_->GetParentInstitution();
  EXPECT_EQ(default_inst, converdyn_);
}

TEST_F(NestedHierarchyTests, CrossEntityRelationships) {
  // Test relationships between different entity types in the hierarchy
  
  // Test facility-to-facility relationships
  // Storage facility should find conversion facility as its parent (layer 1)
  Facility* storage_parent = storage_facility_->GetParentFacility(1);
  EXPECT_EQ(storage_parent, conversion_facility_);
  
  // Conversion facility should find nullptr (no parent facility)
  Facility* conversion_parent = conversion_facility_->GetParentFacility(1);
  EXPECT_EQ(conversion_parent, nullptr);
  
  // Test institution-to-institution relationships
  // ConverDyn should find Honeywell as its parent institution (layer 1)
  Institution* converdyn_parent = converdyn_->GetParentInstitution(1);
  EXPECT_EQ(converdyn_parent, honeywell_);
  
  // Honeywell should find nullptr (no parent institution)
  Institution* honeywell_parent = honeywell_->GetParentInstitution(1);
  EXPECT_EQ(honeywell_parent, nullptr);
  
  // Test region-to-region relationships
  // Metropolis should find Illinois as its parent region (layer 1)
  Region* metropolis_parent = metropolis_->GetParentRegion(1);
  EXPECT_EQ(metropolis_parent, illinois_);
  
  // Illinois should find USA as its parent region (layer 1)
  Region* illinois_parent = illinois_->GetParentRegion(1);
  EXPECT_EQ(illinois_parent, usa_);
  
  // USA should find nullptr (no parent region)
  Region* usa_parent = usa_->GetParentRegion(1);
  EXPECT_EQ(usa_parent, nullptr);
  
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
}

}  // namespace cyclus
