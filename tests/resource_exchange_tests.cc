
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
#include "facility_model.h"

using std::set;
using std::string;
using cyclus::Composition;
using cyclus::Context;
using cyclus::FacilityModel;
using cyclus::Material;
using cyclus::Model;
using cyclus::TestContext;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::ResourceExchange;

class Requester: public MockFacility {
 public:
  Requester(Context* ctx, Request<Material>::Ptr r, int i = 1)
      : MockFacility(ctx),
        Model(ctx),
        r_(r),
        i_(i)
  { };

  virtual cyclus::Model* Clone() {
    Requester* m = new Requester(*this);
    m->InitFrom(this);
    m->r_ = r_;
    m->i_ = i_;
    return m;
  };
  
  set< RequestPortfolio<Material> > AddMatlRequests() {
    set< RequestPortfolio<Material> > rps;
    RequestPortfolio<Material> rp;
    for (int i = 0; i < i_; i++) {
      rp.AddRequest(r_);
    }
    rps.insert(rp);
    return rps;
  }

  Request<Material>::Ptr r_;
  int i_;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ResourceExchangeTests: public ::testing::Test {
 protected:
  TestContext tc;
  Requester* fac;
  ResourceExchange<Material>* exchng;
  string commod;
  double pref;
  Material::Ptr mat;
  Request<Material>::Ptr req;
  
  virtual void SetUp() {
    commod = "name";
    pref = 2.4;
    cyclus::CompMap cm;
    cm[92235] = 1.0;
    Composition::Ptr comp = Composition::CreateFromMass(cm);
    double qty = 1.0;
    mat = Material::CreateUntracked(qty, comp);

    req = Request<Material>::Ptr(new Request<Material>());
    req->commodity = commod;
    req->preference = pref;
    req->target = mat;
    
    exchng = new ResourceExchange<Material>(tc.get());

  };
  
  virtual void TearDown() {
    delete exchng;
  };
  
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, empty) {
  EXPECT_TRUE(exchng->requests.empty());
  exchng->CollectRequests();
  EXPECT_TRUE(exchng->requests.empty());
  exchng->CollectBids();
  EXPECT_TRUE(exchng->bids.empty());
}  

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, cloning) {
  fac = new Requester(tc.get(), req);
  EXPECT_EQ(req, fac->r_);

  FacilityModel* clone = dynamic_cast<FacilityModel*>(fac->Clone());
  clone->Deploy(clone);

  Requester* cast = dynamic_cast<Requester*>(clone);
  EXPECT_EQ(req, cast->r_);
  
  clone->Decommission();
  delete fac;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, 1req) {
  fac = new Requester(tc.get(), req);
  FacilityModel* clone = dynamic_cast<FacilityModel*>(fac->Clone());
  clone->Deploy(clone);
  
  EXPECT_TRUE(exchng->requests.empty());
  exchng->CollectRequests();
  EXPECT_EQ(1, exchng->requests.size());

  RequestPortfolio<Material>& rp =
      const_cast<RequestPortfolio<Material>&>(*exchng->requests.begin());
  EXPECT_EQ(1, rp.requests().size());
  const Request<Material>::Ptr r = *rp.requests().begin();

  EXPECT_EQ(req, r);

  clone->Decommission();
  delete fac;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, Nreq) {
  int nMats = 5;
  fac = new Requester(tc.get(), req, nMats);
  FacilityModel* clone = dynamic_cast<FacilityModel*>(fac->Clone());
  clone->Deploy(clone);
  
  EXPECT_TRUE(exchng->requests.empty());
  exchng->CollectRequests();
  EXPECT_EQ(1, exchng->requests.size());

  RequestPortfolio<Material>& rp =
      const_cast<RequestPortfolio<Material>&>(*exchng->requests.begin());
  EXPECT_EQ(nMats, rp.requests().size());

  std::vector<Request<Material>::Ptr>::const_iterator it;
  const std::vector<Request<Material>::Ptr>& vr = rp.requests();
  for (it = vr.begin(); it != vr.end(); ++it) {
    const Request<Material>::Ptr r = *it;  
    EXPECT_EQ(req, r);
  }
  
  clone->Decommission();
  delete fac;
}
