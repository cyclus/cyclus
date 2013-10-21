
#include <gtest/gtest.h>

#include <set>
#include <string>

#include "resource_exchange.h"

#include "composition.h"
#include "test_context.h"
#include "request.h"
#include "request_portfolio.h"
#include "mock_facility.h"
#include "material.h"
#include "model.h"

using std::set;
using std::string;
using cyclus::Composition;
using cyclus::Context;
using cyclus::Material;
using cyclus::Model;
using cyclus::TestContext;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::ResourceExchange;

class Requester: public MockFacility {
 public:
  Requester(Context* ctx, string commod, double pref, Material::Ptr mat, int i = 1)
      : MockFacility(ctx),
        Model(ctx),
        commod_(commod),
        pref_(pref),
        mat_(mat),
        i_(i)
  { };

  set< RequestPortfolio<Material> > AddMatlRequests() {
    set< RequestPortfolio<Material> > rps;
    RequestPortfolio<Material> rp;
    for (int i = 0; i < i_; i++) {
      Request<Material> r;
      r.target = mat_;
      r.commodity = commod_;
      r.preference = pref_;
      r.requester = this;
      rp.AddRequest(r);
    }
    rps.insert(rp);
    return rps;
  }

 private:
  Material::Ptr mat_;
  int i_;
  string commod_;
  double pref_;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ResourceExchangeTests: public ::testing::Test {
 protected:
  TestContext tc;
  Requester* fac;
  ResourceExchange<Material>* exchng;
  int nMats;

  virtual void SetUp() {
    nMats = 1;
    string commod = "name";
    double pref = 2.4;
    cyclus::CompMap cm;
    cm[92235] = 1.0;
    Composition::Ptr comp = Composition::CreateFromMass(cm);
    double qty = 1.0;
    Material::Ptr mat = Material::Create(tc.get(), qty, comp);
    fac = new Requester(tc.get(), commod, pref, mat, nMats);
    exchng = new ResourceExchange<Material>(tc.get());
  };
  
  virtual void TearDown() {
    delete fac;
    delete exchng;
  };
  
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, exchangers) {
  EXPECT_TRUE(true);
}  
