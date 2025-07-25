#include <gtest/gtest.h>

#include "context.h"
#include "recorder.h"
#include "timer.h"
#include "material.h"
#include "product.h"
#include "composition.h"
#include "region.h"

using cyclus::Material;
using cyclus::Product;
using cyclus::Resource;
using cyclus::Package;

class Dummy : public cyclus::Region {
 public:
  Dummy(cyclus::Context* ctx) : cyclus::Region(ctx) {}
  Dummy* Clone() { return NULL; }
};

class ResourceTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    ctx = new cyclus::Context(&ti, &rec);

    cyclus::CompMap v; v[922350000] = 1;
    cyclus::Composition::Ptr c = cyclus::Composition::CreateFromMass(v);
    cyclus::Agent* dummy = new Dummy(ctx);

    m1 = Material::Create(dummy, 3, c, Package::unpackaged_name(), 10);
    m2 = Material::Create(dummy, 7, c, Package::unpackaged_name(), 20);
    m3 = Material::Create(dummy, 7, c);
    p1 = Product::Create(dummy, 3, "bananas", Package::unpackaged_name(), 10);
    p2 = Product::Create(dummy, 7, "bananas", Package::unpackaged_name(), 20);
    p3 = Product::Create(dummy, 7, "bananas");

    mlimit = Material::Create(dummy, 1e5, c);

    // above limit but not yet overflow
    double mmax_qty = static_cast<double>(0.95*std::numeric_limits<int>::max());
    mmax = Material::Create(dummy, mmax_qty, c);
    // overflow limit
    pmax = Product::Create(dummy, cyclus::CY_LARGE_DOUBLE, "bananas");
  }

  virtual void TearDown() {
    delete ctx;
  }

  cyclus::Timer ti;
  cyclus::Recorder rec;
  cyclus::Context* ctx;
  Material::Ptr m1;
  Material::Ptr m2;
  Material::Ptr m3;
  Material::Ptr mlimit;
  Material::Ptr mmax;
  Product::Ptr p1;
  Product::Ptr p2;
  Product::Ptr p3;
  Product::Ptr pmax;
};

TEST_F(ResourceTest, MaterialAbsorbTrackid) {
  int obj_id = m1->obj_id();
  m1->Absorb(m2);
  EXPECT_EQ(obj_id, m1->obj_id());
}

TEST_F(ResourceTest, MaterialAbsorbGraphid) {
  int state_id = m1->state_id();
  m1->Absorb(m2);
  EXPECT_LT(state_id, m1->state_id());
}

TEST_F(ResourceTest, MaterialExtractTrackid) {
  int obj_id = m1->obj_id();
  Material::Ptr m4 = m1->ExtractQty(2);
  EXPECT_EQ(obj_id, m1->obj_id());
  EXPECT_LT(obj_id, m4->obj_id());
}

TEST_F(ResourceTest, MaterialExtractGraphid) {
  int state_id = m1->state_id();
  Material::Ptr m4 = m1->ExtractQty(2);
  EXPECT_LT(state_id, m1->state_id());
  EXPECT_LT(state_id, m4->state_id());
  EXPECT_NE(m1->state_id(), m4->state_id());
}

TEST_F(ResourceTest, MaterialUnitValue) {
  EXPECT_EQ(m1->UnitValue(), 10);
  EXPECT_EQ(m2->UnitValue(), 20);
  m1->Absorb(m2);
  EXPECT_EQ(m1->UnitValue(), 17);
  EXPECT_EQ(m3->UnitValue(), 0.0);
  Material::Ptr m4 = boost::dynamic_pointer_cast<Material>(m1->Clone());
  EXPECT_EQ(m4->UnitValue(), 17);
  Material::Ptr m5 = m1->ExtractQty(1);
  EXPECT_EQ(m5->UnitValue(), 17);
}

TEST_F(ResourceTest, ProductAbsorbTrackid) {
  int obj_id = p1->obj_id();
  p1->Absorb(p2);
  EXPECT_EQ(obj_id, p1->obj_id());
}

TEST_F(ResourceTest, ProductAbsorbGraphid) {
  int state_id = p1->state_id();
  p1->Absorb(p2);
  EXPECT_LT(state_id, p1->state_id());
}

TEST_F(ResourceTest, ProductExtractTrackid) {
  int obj_id = p1->obj_id();
  Product::Ptr p4 = p1->Extract(2);
  EXPECT_EQ(obj_id, p1->obj_id());
  EXPECT_LT(obj_id, p4->obj_id());
}

TEST_F(ResourceTest, ProductExtractGraphid) {
  int state_id = p1->state_id();
  Product::Ptr p4 = p1->Extract(2);
  EXPECT_LT(state_id, p1->state_id());
  EXPECT_LT(state_id, p4->state_id());
  EXPECT_NE(p1->state_id(), p4->state_id());
}

TEST_F(ResourceTest, ProductUnitValue) {
  EXPECT_EQ(p1->UnitValue(), 10);
  EXPECT_EQ(p2->UnitValue(), 20);
  p1->Absorb(p2);
  EXPECT_EQ(p1->UnitValue(), 17);
  EXPECT_EQ(p3->UnitValue(), 0.0);
  Product::Ptr p4 = boost::dynamic_pointer_cast<Product>(p1->Clone());
  EXPECT_EQ(p4->UnitValue(), 17);
  Product::Ptr p5 = p1->Extract(1);
  EXPECT_EQ(p5->UnitValue(), 17);
}

TEST_F(ResourceTest, DefaultPackageId) {
  EXPECT_EQ(m1->package_name(), Package::unpackaged_name());
  EXPECT_EQ(m2->package_name(), Package::unpackaged_name());
  EXPECT_EQ(p1->package_name(), Package::unpackaged_name());
  EXPECT_EQ(p2->package_name(), Package::unpackaged_name());

  Product::Ptr p4 = p1->Extract(2);
  EXPECT_EQ(p4->package_name(), Package::unpackaged_name());
}

TEST_F(ResourceTest, ChangePackage) {
  ctx->AddPackage("foo", 1, 5, "first");
  Package::Ptr pkg = ctx->GetPackage("foo");
  std::string pkg_name = pkg->name();

  Product::Ptr p4 = p1->Extract(2);
  p4->ChangePackage(pkg_name);
  EXPECT_EQ(p4->package_name(), pkg_name);
  EXPECT_EQ(p1->package_name(), Package::unpackaged_name());

  m1->ChangePackage(pkg_name);
  EXPECT_EQ(m1->package_name(), pkg_name);
}

TEST_F(ResourceTest, PackageResource) {
  ctx->AddPackage("foo", 1, 5, "first");
  Package::Ptr pkg = ctx->GetPackage("foo");
  std::string pkg_name = pkg->name();

  // nothing packaged
  Product::Ptr p4 = p1->Extract(0.5);
  std::vector<Product::Ptr> p4_pkgd = p4->Package<Product>(pkg);

  // everything stays in old product, with same (default) package id
  EXPECT_EQ(p4->package_name(), Package::unpackaged_name());
  EXPECT_EQ(p4->quantity(), 0.5);

  // all packaged
  std::vector<Product::Ptr> p1_pkgd = p1->Package<Product>(pkg);
  EXPECT_EQ(p1->quantity(), 0);
  EXPECT_EQ(p1_pkgd.size(), 1);
  EXPECT_EQ(p1_pkgd[0]->package_name(), pkg_name);

  // // two packages
  std::vector<Product::Ptr> p2_pkgd = p2->Package<Product>(pkg);
  EXPECT_EQ(p2->quantity(), 0);
  EXPECT_EQ(p2_pkgd.size(), 2);
  EXPECT_EQ(p2_pkgd[0]->quantity(), 5);
  EXPECT_EQ(p2_pkgd[1]->quantity(), 2);
  EXPECT_EQ(p2_pkgd[0]->package_name(), pkg_name);
  EXPECT_EQ(p2_pkgd[1]->package_name(), pkg_name);

  Material::Ptr m4 = m2->ExtractQty(5.5);
  std::vector<Material::Ptr> m4_pkgd = m4->Package<Material>(pkg);
  EXPECT_EQ(m4->package_name(), Package::unpackaged_name());
  EXPECT_EQ(m4->quantity(), 0.5);
  EXPECT_EQ(m4_pkgd.size(), 1);
  EXPECT_EQ(m4_pkgd[0]->package_name(), pkg_name);
  EXPECT_EQ(m4_pkgd[0]->quantity(), 5);
}

TEST_F(ResourceTest, RepackageLimit) {
  ctx->AddPackage("foo", 0.5, 1, "first");
  Package::Ptr pkg = ctx->GetPackage("foo");
  std::string pkg_name = pkg->name();

  // warn but no throw
  EXPECT_NO_THROW(mlimit->Package<Material>(pkg));

  // over limit, throw
  EXPECT_THROW(mmax->Package<Material>(pkg), cyclus::ValueError);
  EXPECT_THROW(pmax->Package<Product>(pkg), cyclus::ValueError);
}
