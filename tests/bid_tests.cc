
#include <gtest/gtest.h>

#include <string>

#include "composition.h"
#include "facility.h"
#include "generic_resource.h"
#include "material.h"
#include "test_modules/test_facility.h"
#include "request.h"
#include "resource_helpers.h"
#include "test_context.h"

#include "bid.h"

using cyclus::Bid;
using cyclus::Composition;
using cyclus::GenericResource;
using cyclus::Material;
using cyclus::Request;
using cyclus::TestContext;
using std::string;
using test_helpers::get_bid;
using test_helpers::get_mat;
using test_helpers::get_req;
using test_helpers::trader;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(BidTests, MaterialGetSet) {
  TestContext tc;
  TestFacility* fac = new TestFacility(tc.get());
  cyclus::CompMap cm;
  cm[92235] = 1.0;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  double qty = 1.0;
  Material::Ptr mat = Material::CreateUntracked(qty, comp);
  Request<Material>::Ptr req = get_req();
  
  Bid<Material>::Ptr r = Bid<Material>::Create(req, mat, fac);

  EXPECT_EQ(fac, r->bidder());
  EXPECT_EQ(req, r->request());
  EXPECT_EQ(mat, r->offer());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(BidTests, GenRsrcGetSet) {
  TestContext tc;
  TestFacility* fac = new TestFacility(tc.get());
  double qty = 1.0;
  string quality = "qual";

  GenericResource::Ptr rsrc =
      GenericResource::CreateUntracked(qty, quality);
  
  Request<GenericResource>::Ptr req =
      Request<GenericResource>::Create(rsrc, trader);
  
  Bid<GenericResource>::Ptr r = Bid<GenericResource>::Create(req, rsrc, fac);

  EXPECT_EQ(fac, r->bidder());
  EXPECT_EQ(req, r->request());
  EXPECT_EQ(rsrc, r->offer());
}
