#include <string>

#include <gtest/gtest.h>

#include "bid.h"
#include "composition.h"
#include "facility.h"
#include "material.h"
#include "product.h"
#include "request.h"
#include "resource_helpers.h"
#include "test_context.h"
#include "test_modules/test_facility.h"

using cyclus::Bid;
using cyclus::Composition;
using cyclus::Product;
using cyclus::Material;
using cyclus::Request;
using cyclus::TestContext;
using std::string;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(BidTests, MaterialGetSet) {
  TestContext tc;
  TestFacility* fac = tc.trader();
  cyclus::CompMap cm;
  cm[92235] = 1.0;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  double qty = 1.0;
  Material::Ptr mat = tc.mat();
  Request<Material>* req = tc.NewReq(); 
  Bid<Material>* bid = Bid<Material>::Create(req, mat, fac);

  EXPECT_EQ(fac, bid->bidder());
  EXPECT_EQ(req, bid->request());
  EXPECT_EQ(mat, bid->offer());

  delete bid;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(BidTests, ProductGetSet) {
  TestContext tc;
  TestFacility* fac = tc.trader();
  double qty = 1.0;
  string quality = "qual";

  Product::Ptr rsrc = Product::CreateUntracked(qty, quality);

  Request<Product>* req = Request<Product>::Create(rsrc, fac);

  Bid<Product>* bid = Bid<Product>::Create(req, rsrc, fac);

  EXPECT_EQ(fac, bid->bidder());
  EXPECT_EQ(req, bid->request());
  EXPECT_EQ(rsrc, bid->offer());

  delete bid;
  delete req;
}
