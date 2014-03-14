
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
using cyclus::Converter;
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
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct RsrcQtyConverter : public Converter<Resource> {
  RsrcQtyConverter() {}
  virtual ~RsrcQtyConverter() {}
  
  virtual double convert(Resource::Ptr r) {
    return r->quantity() * fraction;
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct MatQualConverter : public Converter<Material> {
  MatQualConverter() {}
  virtual ~MatQualConverter() {}
  
  virtual double convert(Material::Ptr r) {
    const CompMap& comp = r->comp()->mass();
    double uamt = comp.find(u235)->second;
    return comp.find(u235)->second * fraction;
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct GenRsrcQualConverter : public Converter<GenericResource> {
  GenRsrcQualConverter() {}
  virtual ~GenRsrcQualConverter() {}
  
  virtual double convert(GenericResource::Ptr r) {
    if (r->quality().compare(quality) == 0) {
      return val;
    } else {
      return 0.0;
    }
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, RsrcGetSet) {
  Converter<Resource>::Ptr c(new RsrcQtyConverter());
  CapacityConstraint<Resource> cc(val, c);
  
  EXPECT_EQ(val, cc.capacity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, Trivial) {
  CapacityConstraint<Resource> cc(val);
  double val = 42;
  EXPECT_EQ(val, cc.convert(cyclus::NewBlankMaterial(val))); // some magic number
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, Equality) {
  CapacityConstraint<Resource> cc1(val);
  CapacityConstraint<Resource> cc2(val);
  EXPECT_EQ(cc1, cc2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, RsrcQty) {
  TestContext tc;
  cyclus::CompMap cm;
  double qty = quantity;
  cm[92235] = val;
  Composition::Ptr comp = Composition::CreateFromMass(cm);

  Converter<Resource>::Ptr c(new RsrcQtyConverter());
  CapacityConstraint<Resource> cc(val, c);
  
  Material::Ptr mat = Material::CreateUntracked(qty, comp);
  EXPECT_DOUBLE_EQ(cc.convert(mat), qty*fraction);

  qty = std::rand();
  mat = Material::CreateUntracked(qty, comp);
  EXPECT_DOUBLE_EQ(cc.convert(mat), qty*fraction);

  string s = "";
  GenericResource::Ptr gr = GenericResource::CreateUntracked(qty, s);
  EXPECT_DOUBLE_EQ(cc.convert(gr), qty*fraction);

  qty = std::rand();
  gr = GenericResource::CreateUntracked(qty, s);
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
  
  Converter<Material>::Ptr c(new MatQualConverter());
  CapacityConstraint<Material> cc(val, c);
  
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
TEST(CapacityConstraintTests, GenGenRsrcQuality) {
  TestContext tc;
  double quan = 4.0;
  string qual = quality;
  
  Converter<GenericResource>::Ptr c(new GenRsrcQualConverter());
  CapacityConstraint<GenericResource> cc(val, c);
  
  GenericResource::Ptr gr = GenericResource::CreateUntracked(quan, qual);
  EXPECT_DOUBLE_EQ(cc.convert(gr), val);

  gr = GenericResource::CreateUntracked(quan, "foo");
  EXPECT_DOUBLE_EQ(cc.convert(gr), 0.0);
}
