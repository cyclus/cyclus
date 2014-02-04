
#include <gtest/gtest.h>

#include <string>

#include "composition.h"
#include "facility_model.h"
#include "generic_resource.h"
#include "material.h"
#include "mock_facility.h"
#include "test_context.h"
#include "trader.h"

#include "request.h"

using cyclus::Composition;
using cyclus::GenericResource;
using cyclus::Material;
using cyclus::Request;
using cyclus::TestContext;
using cyclus::Trader;
using std::string;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RequestTests, MaterialGetSet) {
  TestContext tc;
  MockFacility* fac = new MockFacility(tc.get());
  Trader* excast = dynamic_cast<Trader*>(fac);
  
  string commod = "name";
  double pref = 2.4;
  cyclus::CompMap cm;
  cm[92235] = 1.0;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  double qty = 1.0;
  Material::Ptr mat = Material::CreateUntracked(qty, comp);
  
  Request<Material>::Ptr r = Request<Material>::Create(mat, fac, commod, pref);

  EXPECT_EQ(commod, r->commodity());
  EXPECT_EQ(excast, r->requester());
  EXPECT_EQ(mat, r->target());
  EXPECT_EQ(pref, r->preference());
  
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
  
  Request<GenericResource>::Ptr r =
      Request<GenericResource>::Create(rsrc, fac, commod, pref);

  EXPECT_EQ(commod, r->commodity());
  EXPECT_EQ(fac, r->requester());
  EXPECT_EQ(rsrc, r->target());
  EXPECT_EQ(pref, r->preference());
  
}
