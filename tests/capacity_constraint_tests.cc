
#include <gtest/gtest.h>

#include <math.h>
#include <string>
#include <utility>

#include "composition.h"
#include "cyc_limits.h"
#include "generic_resource.h"
#include "material.h"
#include "resource.h"
#include "test_context.h"

#include "capacity_constraint.h"

using std::string;
using cyclus::CapacityConstraint;
using cyclus::Composition;
using cyclus::CompMap;
using cyclus::GenericResource;
using cyclus::Material;
using cyclus::Resource;
using cyclus::TestContext;

static double fraction = 0.5;
static int u235 = 92235;
static double quantity = 2.7;
static double val = 9.4;
static string quality = "qual";

double rsrc_quantity_converter(Resource::Ptr r) {
  return r->quantity() * fraction;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, RsrcGetSet) {

  CapacityConstraint<Resource> cc;
  cc.capacity = val;
  cc.converter = &rsrc_quantity_converter;
  
  EXPECT_EQ(val, cc.capacity);
  EXPECT_EQ(&rsrc_quantity_converter, cc.converter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, RsrcQty) {
  TestContext tc;
  cyclus::CompMap cm;
  double qty = quantity;
  cm[92235] = val;
  Composition::Ptr comp = Composition::CreateFromMass(cm);

  CapacityConstraint<Resource> cc;
  cc.capacity = val;
  cc.converter = &rsrc_quantity_converter;
  
  Material::Ptr mat = Material::CreateUntracked(qty, comp);
  EXPECT_DOUBLE_EQ((cc.converter)(mat), qty*fraction);

  qty = std::rand();
  mat = Material::CreateUntracked(qty, comp);
  EXPECT_DOUBLE_EQ((cc.converter)(mat), qty*fraction);

  string s = "";
  GenericResource::Ptr gr = GenericResource::CreateUntracked(qty, s, s);
  EXPECT_DOUBLE_EQ((cc.converter)(gr), qty*fraction);

  qty = std::rand();
  gr = GenericResource::CreateUntracked(qty, s, s);
  EXPECT_DOUBLE_EQ((cc.converter)(gr), qty*fraction);
}

double mat_quality_converter(Material::Ptr m) {
  const CompMap& comp = m->comp()->mass();
  double uamt = comp.find(u235)->second;
  return comp.find(u235)->second * fraction;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, MaterialGetSet) {

  CapacityConstraint<Material> cc;
  cc.capacity = val;
  cc.converter = &mat_quality_converter;
  
  EXPECT_EQ(val, cc.capacity);
  EXPECT_EQ(&mat_quality_converter, cc.converter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, MaterialQuality) {
  TestContext tc;
  cyclus::CompMap cm;
  double qty = quantity;
  cm[92235] = val;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  Material::Ptr mat = Material::CreateUntracked(qty, comp);
  
  CapacityConstraint<Material> cc;
  cc.capacity = val;
  cc.converter = &mat_quality_converter;
  
  EXPECT_DOUBLE_EQ((cc.converter)(mat), val*fraction);
  
  cm[92235] = val*fraction;
  comp = Composition::CreateFromMass(cm);
  mat = Material::CreateUntracked(qty, comp);

  EXPECT_DOUBLE_EQ((cc.converter)(mat), val*fraction*fraction);

  cm[92235] = 0.0;
  comp = Composition::CreateFromMass(cm);
  mat = Material::CreateUntracked(qty, comp);

  EXPECT_DOUBLE_EQ((cc.converter)(mat), 0.0);
}

double gen_rsrc_quality_converter(GenericResource::Ptr gr) {
  if (gr->quality().compare(quality) == 0) {
    return val;
  } else {
    return 0.0;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, GenRsrcGetSet) {

  CapacityConstraint<GenericResource> cc;
  cc.capacity = val;
  cc.converter = &gen_rsrc_quality_converter;
  
  EXPECT_EQ(val, cc.capacity);
  EXPECT_EQ(&gen_rsrc_quality_converter, cc.converter);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, GenRsrcQuality) {
  TestContext tc;
  double quan = 4.0;
  string units = "kg";
  string qual = quality;
  
  CapacityConstraint<GenericResource> cc;
  cc.capacity = val;
  cc.converter = &gen_rsrc_quality_converter;
  
  GenericResource::Ptr gr = GenericResource::CreateUntracked(quan, qual, units);
  EXPECT_DOUBLE_EQ((cc.converter)(gr), val);

  gr = GenericResource::CreateUntracked(quan, units, units);
  EXPECT_DOUBLE_EQ((cc.converter)(gr), 0.0);
}
