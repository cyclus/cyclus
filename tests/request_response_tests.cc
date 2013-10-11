
#include <gtest/gtest.h>

#include <string>

#include "composition.h"
#include "facility_model.h"
#include "generic_resource.h"
#include "material.h"
#include "test_context.h"
#include "mock_facility.h"
#include "request.h"

#include "request_response.h"

using std::string;
using cyclus::Composition;
using cyclus::GenericResource;
using cyclus::Material;
using cyclus::Request;
using cyclus::RequestResponse;
using cyclus::TestContext;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ResponseRequestTests, MaterialGetSet) {
  TestContext tc;
  MockFacility* fac = new MockFacility(tc.get());
  cyclus::CompMap cm;
  cm[92235] = 1.0;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  double qty = 1.0;
  Material::Ptr mat = Material::Create(tc.get(), qty, comp);
  Request<Material> req;
  
  RequestResponse<Material> r;
  r.responder = fac;
  r.request = &req;
  r.response = mat;

  EXPECT_EQ(fac, r.responder);
  EXPECT_EQ(&req, r.request);
  EXPECT_EQ(mat, r.response);
  
  delete fac;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ResponseRequestTests, GenRsrcGetSet) {
  TestContext tc;
  MockFacility* fac = new MockFacility(tc.get());
  double qty = 1.0;
  string quality = "qual";
  string units = "units";

  GenericResource::Ptr rsrc =
      GenericResource::Create(tc.get(), qty, quality, units);
  Request<GenericResource> req;
  
  RequestResponse<GenericResource> r;
  r.responder = fac;
  r.request = &req;
  r.response = rsrc;

  EXPECT_EQ(fac, r.responder);
  EXPECT_EQ(&req, r.request);
  EXPECT_EQ(rsrc, r.response);
  
  delete fac;
}
