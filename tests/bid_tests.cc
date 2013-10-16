
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
  Material::Ptr mat = Material::Create(tc.get(), qty, comp);
  Request<Material> req;
  
  Bid<Material> r;
  r.responder = fac;
  r.request = &req;
  r.bid = mat;

  EXPECT_EQ(fac, r.responder);
  EXPECT_EQ(&req, r.request);
  EXPECT_EQ(mat, r.bid);
  
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
      GenericResource::Create(tc.get(), qty, quality, units);
  Request<GenericResource> req;
  
  Bid<GenericResource> r;
  r.responder = fac;
  r.request = &req;
  r.bid = rsrc;

  EXPECT_EQ(fac, r.responder);
  EXPECT_EQ(&req, r.request);
  EXPECT_EQ(rsrc, r.bid);
  
  delete fac;
}
