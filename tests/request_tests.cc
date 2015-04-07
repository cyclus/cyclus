#include <string>

#include <gtest/gtest.h>

#include "composition.h"
#include "facility.h"
#include "material.h"
#include "product.h"
#include "request.h"
#include "test_context.h"
#include "test_agents/test_facility.h"
#include "trader.h"

using cyclus::Composition;
using cyclus::Product;
using cyclus::Material;
using cyclus::Request;
using cyclus::TestContext;
using cyclus::Trader;
using std::string;

TEST(RequestTests, Defaults) {
  TestContext tc;
  TestFacility* fac = tc.trader();
  Trader* excast = dynamic_cast<Trader*>(fac);

  cyclus::CompMap cm;
  cm[92235] = 1.0;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  double qty = 1.0;
  Material::Ptr mat = Material::CreateUntracked(qty, comp);

  Request<Material>* r = Request<Material>::Create(mat, fac);

  EXPECT_EQ("", r->commodity());
  EXPECT_FALSE(r->exclusive());
  EXPECT_EQ(1., r->preference());

  delete r;
}

TEST(RequestTests, MaterialGetSet) {
  TestContext tc;
  TestFacility* fac = tc.trader();
  Trader* excast = dynamic_cast<Trader*>(fac);

  string commod = "name";
  double pref = 2.4;
  cyclus::CompMap cm;
  cm[92235] = 1.0;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  double qty = 1.0;
  Material::Ptr mat = Material::CreateUntracked(qty, comp);

  Request<Material>* r = Request<Material>::Create(mat, fac, commod, pref);

  EXPECT_EQ(commod, r->commodity());
  EXPECT_EQ(excast, r->requester());
  EXPECT_EQ(mat, r->target());
  EXPECT_EQ(pref, r->preference());

  delete r;
}

TEST(RequestTests, ProductGetSet) {
  TestContext tc;
  TestFacility* fac = tc.trader();
  string commod = "name";
  double pref = 2.4;
  double qty = 1.0;
  string quality = "qual";

  Product::Ptr rsrc =
      Product::CreateUntracked(qty, quality);

  Request<Product>* r =
      Request<Product>::Create(rsrc, fac, commod, pref);

  EXPECT_EQ(commod, r->commodity());
  EXPECT_EQ(fac, r->requester());
  EXPECT_EQ(rsrc, r->target());
  EXPECT_EQ(pref, r->preference());

  delete r;
}
