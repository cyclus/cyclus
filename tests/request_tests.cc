
#include <gtest/gtest.h>

#include <string>

#include "composition.h"
#include "material.h"
#include "test_context.h"
#include "mock_facility.h"

#include "request.h"

using std::string;
using cyclus::Composition;
using cyclus::Material;
using cyclus::Request;
using cyclus::TestContext;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RequestTests, GetSet) {
  TestContext tc;
  MockFacility* fac = new MockFacility(tc.get());
  string commod = "name";
  double pref = 2.4;
  cyclus::CompMap cm;
  cm[92235] = 1.0;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  double mat_qty = 1.0;
  Material::Ptr mat = Material::Create(tc.get(), mat_qty, comp);
  
  Request<Material> r;
  r.commodity = commod;
  r.requester = fac;
  r.target = mat.get();
  r.preference = pref;

  EXPECT_EQ(commod, r.commodity);
  EXPECT_EQ(fac, r.requester);
  EXPECT_EQ(mat.get(), r.target);
  EXPECT_EQ(pref, r.preference);
  
  delete fac;
}
