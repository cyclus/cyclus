
#include <gtest/gtest.h>

#include <string>

#include "composition.h"
#include "facility_model.h"
#include "generic_resource.h"
#include "material.h"
#include "test_context.h"
#include "mock_facility.h"
#include "request.h"

#include "bid.h"

using std::string;
using cyclus::Composition;
using cyclus::GenericResource;
using cyclus::Material;
using cyclus::Request;
using cyclus::Bid;
using cyclus::TestContext;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(BidTests, MaterialGetSet) {
  TestContext tc;
  MockFacility* fac = new MockFacility(tc.get());
  cyclus::CompMap cm;
  cm[92235] = 1.0;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  double qty = 1.0;
  Material::Ptr mat = Material::CreateUntracked(qty, comp);
  Request<Material>::Ptr req = Request<Material>::Ptr(new Request<Material>());
  
  Bid<Material> r;
  r.bidder = fac;
  r.request = req;
  r.offer = mat;

  EXPECT_EQ(fac, r.bidder);
  EXPECT_EQ(req, r.request);
  EXPECT_EQ(mat, r.offer);
  
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
  Request<GenericResource>::Ptr req =
      Request<GenericResource>::Ptr(new Request<GenericResource>());
  
  Bid<GenericResource> r;
  r.bidder = fac;
  r.request = req;
  r.offer = rsrc;

  EXPECT_EQ(fac, r.bidder);
  EXPECT_EQ(req, r.request);
  EXPECT_EQ(rsrc, r.offer);
  
  delete fac;
}
