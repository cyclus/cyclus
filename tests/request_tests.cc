
#include <gtest/gtest.h>

#include <string>

#include "composition.h"
#include "facility_model.h"
#include "generic_resource.h"
#include "material.h"
#include "test_context.h"
#include "mock_facility.h"
#include "exchanger.h"

#include "request.h"

using std::string;
using cyclus::Composition;
using cyclus::GenericResource;
using cyclus::Material;
using cyclus::Request;
using cyclus::TestContext;
using cyclus::Exchanger;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RequestTests, MaterialGetSet) {
  TestContext tc;
  MockFacility* fac = new MockFacility(tc.get());
  Exchanger* excast = dynamic_cast<Exchanger*>(fac);
  
  string commod = "name";
  double pref = 2.4;
  cyclus::CompMap cm;
  cm[92235] = 1.0;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  double qty = 1.0;
  Material::Ptr mat = Material::CreateUntracked(qty, comp);
  
  Request<Material> r;
  r.commodity = commod;
  r.requester = fac;
  r.target = mat;//.get();
  r.preference = pref;

  EXPECT_EQ(commod, r.commodity);
  EXPECT_EQ(excast, r.requester);
  EXPECT_EQ(mat, r.target);
  EXPECT_EQ(pref, r.preference);
  
  delete fac;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RequestTests, GenRsrcGetSet) {
  TestContext tc;
  MockFacility* fac = new MockFacility(tc.get());
  string commod = "name";
  double pref = 2.4;
  double qty = 1.0;
  string quality = "qual";
  string units = "units";

  GenericResource::Ptr rsrc =
      GenericResource::CreateUntracked(qty, quality, units);
  
  Request<GenericResource> r;
  r.commodity = commod;
  r.requester = fac;
  r.target = rsrc;//.get();
  r.preference = pref;

  EXPECT_EQ(commod, r.commodity);
  EXPECT_EQ(fac, r.requester);
  EXPECT_EQ(rsrc, r.target);
  EXPECT_EQ(pref, r.preference);
  
  delete fac;
}
