
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

using cyclus::CapacityConstraint;
using cyclus::CompMap;
using cyclus::Composition;
using cyclus::GenericResource;
using cyclus::Material;
using cyclus::Resource;
using cyclus::TestContext;
using std::string;

static double quantity = 2.7;
static double val = 9.4;
static int u235 = 92235;
static std::string quality = "qual";
static double fraction = 0.5;

// some helper functions to use
static double rsrc_quantity_converter(cyclus::Resource* r) {
  return r->quantity() * fraction;
}

static double mat_quality_converter(cyclus::Material* m) {
  const cyclus::CompMap& comp = m->comp()->mass();
  double uamt = comp.find(u235)->second;
  return comp.find(u235)->second * fraction;
}

static double gen_rsrc_quality_converter(cyclus::GenericResource* gr) {
  if (gr->quality().compare(quality) == 0) {
    return val;
  } else {
    return 0.0;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, RsrcGetSet) {
  CapacityConstraint<Resource> cc(val, &rsrc_quantity_converter);
  
  EXPECT_EQ(val, cc.capacity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, Trivial) {
  CapacityConstraint<Resource> cc(val);
  Material* mat;
  EXPECT_EQ(1, cc.convert(mat));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, RsrcQty) {
  TestContext tc;
  cyclus::CompMap cm;
  double qty = quantity;
  cm[92235] = val;
  Composition::Ptr comp = Composition::CreateFromMass(cm);

  CapacityConstraint<Resource> cc(val, &rsrc_quantity_converter);
  
  Material::Ptr mat = Material::CreateUntracked(qty, comp);
  EXPECT_DOUBLE_EQ(cc.convert(mat), qty*fraction);

  qty = std::rand();
  mat = Material::CreateUntracked(qty, comp);
  EXPECT_DOUBLE_EQ(cc.convert(mat), qty*fraction);

  string s = "";
  GenericResource::Ptr gr = GenericResource::CreateUntracked(qty, s, s);
  EXPECT_DOUBLE_EQ(cc.convert(gr), qty*fraction);

  qty = std::rand();
  gr = GenericResource::CreateUntracked(qty, s, s);
  EXPECT_DOUBLE_EQ(cc.convert(gr), qty*fraction);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, MaterialQuality) {
  TestContext tc;
  cyclus::CompMap cm;
  double qty = quantity;
  cm[92235] = val;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  Material::Ptr mat = Material::CreateUntracked(qty, comp);
  
  CapacityConstraint<Material> cc(val, &mat_quality_converter);
  
  EXPECT_DOUBLE_EQ(cc.convert(mat), val*fraction);
  
  cm[92235] = val*fraction;
  comp = Composition::CreateFromMass(cm);
  mat = Material::CreateUntracked(qty, comp);

  EXPECT_DOUBLE_EQ(cc.convert(mat), val*fraction*fraction);

  cm[92235] = 0.0;
  comp = Composition::CreateFromMass(cm);
  mat = Material::CreateUntracked(qty, comp);

  EXPECT_DOUBLE_EQ(cc.convert(mat), 0.0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, GenRsrcQuality) {
  TestContext tc;
  double quan = 4.0;
  string units = "kg";
  string qual = quality;
  
  CapacityConstraint<GenericResource> cc(val, &gen_rsrc_quality_converter);
  
  GenericResource::Ptr gr = GenericResource::CreateUntracked(quan, qual, units);
  EXPECT_DOUBLE_EQ(cc.convert(gr), val);

  gr = GenericResource::CreateUntracked(quan, units, units);
  EXPECT_DOUBLE_EQ(cc.convert(gr), 0.0);
}
