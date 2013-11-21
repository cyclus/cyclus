
#include <gtest/gtest.h>

#include <string>

#include "composition.h"
#include "facility_model.h"
#include "generic_resource.h"
#include "material.h"
#include "mock_facility.h"
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
  MockFacility* fac = new MockFacility(tc.get());
  cyclus::CompMap cm;
  cm[92235] = 1.0;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  double qty = 1.0;
  Material::Ptr mat = Material::CreateUntracked(qty, comp);
  Request<Material>::Ptr req = get_req();
  
  Bid<Material> r(req, mat, fac);

  EXPECT_EQ(fac, r.bidder());
  EXPECT_EQ(req, r.request());
  EXPECT_EQ(mat, r.offer());
  
  delete fac;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(BidTests, GenRsrcGetSet) {
  TestContext tc;
  MockFacility* fac = new MockFacility(tc.get());
  double qty = 1.0;
  string quality = "qual";
  string units = "units";

  GenericResource::Ptr rsrc =
      GenericResource::CreateUntracked(qty, quality, units);
  
  Request<GenericResource>::Ptr req(
      new Request<GenericResource>(rsrc, &trader));
  
  Bid<GenericResource> r(req, rsrc, fac);

  EXPECT_EQ(fac, r.bidder());
  EXPECT_EQ(req, r.request());
  EXPECT_EQ(rsrc, r.offer());
  
  delete fac;
}
