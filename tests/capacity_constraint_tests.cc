
#include <gtest/gtest.h>

#include <math.h>
#include <string>
#include <utility>

#include "composition.h"
#include "cyc_limits.h"
#include "product.h"
#include "material.h"
#include "resource.h"
#include "test_context.h"

#include "capacity_constraint.h"

using cyclus::Arc;
using cyclus::CapacityConstraint;
using cyclus::CompMap;
using cyclus::Composition;
using cyclus::Converter;
using cyclus::Product;
using cyclus::Material;
using cyclus::Resource;
using cyclus::TestContext;
using cyclus::ExchangeTranslationContext;
using cyclus::NewBlankMaterial;

using std::string;

static double quantity = 2.7;
static double val = 9.4;
static int u235 = 92235;
static std::string quality = "qual";
static double fraction = 0.5;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct RsrcQtyConverter : public Converter<Resource> {
  RsrcQtyConverter() {}
  virtual ~RsrcQtyConverter() {}
  
  virtual double convert(
      Resource::Ptr r,
      Arc const * a = NULL,
      ExchangeTranslationContext<Resource> const * ctx = NULL) const {
    return r->quantity() * fraction;
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct MatQualConverter : public Converter<Material> {
  MatQualConverter() {}
  virtual ~MatQualConverter() {}
  
  virtual double convert(
      Material::Ptr r,
      Arc const * a = NULL,
      ExchangeTranslationContext<Material> const * ctx = NULL) const {
    const CompMap& comp = r->comp()->mass();
    double uamt = comp.find(u235)->second;
    return comp.find(u235)->second * fraction;
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct ProductQualConverter : public Converter<Product> {
  ProductQualConverter() {}
  virtual ~ProductQualConverter() {}
  
  virtual double convert(
      Product::Ptr r,
      Arc const * a = NULL,
      ExchangeTranslationContext<Product> const * ctx = NULL) const {
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
  EXPECT_EQ(val, cc.convert(NewBlankMaterial(val))); // some magic number
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
  CompMap cm;
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
  Product::Ptr gr = Product::CreateUntracked(qty, s);
  EXPECT_DOUBLE_EQ(cc.convert(gr), qty*fraction);

  qty = std::rand();
  gr = Product::CreateUntracked(qty, s);
  EXPECT_DOUBLE_EQ(cc.convert(gr), qty*fraction);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(CapacityConstraintTests, MaterialQuality) {
  TestContext tc;
  CompMap cm;
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
TEST(CapacityConstraintTests, GenProductQuality) {
  TestContext tc;
  double quan = 4.0;
  string qual = quality;
  
  Converter<Product>::Ptr c(new ProductQualConverter());
  CapacityConstraint<Product> cc(val, c);
  
  Product::Ptr gr = Product::CreateUntracked(quan, qual);
  EXPECT_DOUBLE_EQ(cc.convert(gr), val);

  gr = Product::CreateUntracked(quan, "foo");
  EXPECT_DOUBLE_EQ(cc.convert(gr), 0.0);
}
