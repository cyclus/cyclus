
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

class Dummy : public cyclus::Region {
 public:
  Dummy(cyclus::Context* ctx) : cyclus::Region(ctx) {};
  Dummy* Clone() { return NULL; };
};

class ResourceTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    ctx = new cyclus::Context(&ti, &rec);

    cyclus::CompMap v; v[922350000] = 1;
    cyclus::Composition::Ptr c = cyclus::Composition::CreateFromMass(v);
    cyclus::Agent* dummy = new Dummy(ctx);

    m1 = Material::Create(dummy, 3, c);
    m2 = Material::Create(dummy, 7, c);
    p1 = Product::Create(dummy, 3, "bananas");
    p2 = Product::Create(dummy, 7, "bananas");
  }

  virtual void TearDown() {
    delete ctx;
  }

  cyclus::Timer ti;
  cyclus::Recorder rec;
  cyclus::Context* ctx;
  Material::Ptr m1;
  Material::Ptr m2;
  Product::Ptr p1;
  Product::Ptr p2;
};

TEST_F(ResourceTest, MaterialAbsorbTrackid) {
  int trackid = m1->trackid();
  m1->Absorb(m2);
  EXPECT_EQ(trackid, m1->trackid());
}

TEST_F(ResourceTest, MaterialAbsorbGraphid) {
  int graphid = m1->graphid();
  m1->Absorb(m2);
  EXPECT_LT(graphid, m1->graphid());
}

TEST_F(ResourceTest, MaterialExtractTrackid) {
  int trackid = m1->trackid();
  Material::Ptr m3 = m1->ExtractQty(2);
  EXPECT_EQ(trackid, m1->trackid());
  EXPECT_LT(trackid, m3->trackid());
}

TEST_F(ResourceTest, MaterialExtractGraphid) {
  int graphid = m1->graphid();
  Material::Ptr m3 = m1->ExtractQty(2);
  EXPECT_LT(graphid, m1->graphid());
  EXPECT_LT(graphid, m3->graphid());
  EXPECT_NE(m1->graphid(), m3->graphid());
}

TEST_F(ResourceTest, ProductAbsorbTrackid) {
  int trackid = p1->trackid();
  p1->Absorb(p2);
  EXPECT_EQ(trackid, p1->trackid());
}

TEST_F(ResourceTest, ProductAbsorbGraphid) {
  int graphid = p1->graphid();
  p1->Absorb(p2);
  EXPECT_LT(graphid, p1->graphid());
}

TEST_F(ResourceTest, ProductExtractTrackid) {
  int trackid = p1->trackid();
  Product::Ptr p3 = p1->Extract(2);
  EXPECT_EQ(trackid, p1->trackid());
  EXPECT_LT(trackid, p3->trackid());
}

TEST_F(ResourceTest, ProductExtractGraphid) {
  int graphid = p1->graphid();
  Product::Ptr p3 = p1->Extract(2);
  EXPECT_LT(graphid, p1->graphid());
  EXPECT_LT(graphid, p3->graphid());
  EXPECT_NE(p1->graphid(), p3->graphid());
}

