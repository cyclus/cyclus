
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
  Requester(Context* ctx, string commod, double pref, Material::Ptr mat, int i = 1)
      : MockFacility(ctx),
        Model(ctx),
        commod_(commod),
        pref_(pref),
        mat_(mat),
        i_(i)
  { };

  virtual cyclus::Model* Clone() {
    Requester* m = new Requester(*this);
    m->InitFrom(this);
    m->mat_ = mat_;
    m->i_ = i_;
    m->pref_ = pref_;
    m->commod_ = commod_;
    return m;
  };
  
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
  string commod;
  double pref;
  Material::Ptr mat;
  
  virtual void SetUp() {
    commod = "name";
    pref = 2.4;
    cyclus::CompMap cm;
    cm[92235] = 1.0;
    Composition::Ptr comp = Composition::CreateFromMass(cm);
    double qty = 1.0;
    mat = Material::CreateUntracked(qty, comp);
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
TEST_F(ResourceExchangeTests, 1req) {
  int nMats = 1;
  fac = new Requester(tc.get(), commod, pref, mat, nMats);
  Requester* clone = dynamic_cast<Requester*>(fac->Clone());
  clone->Deploy(clone);

  Request<Material> req;
  req.commodity = commod;
  req.target = mat;
  req.preference = pref;
  req.requester = clone;
  
  EXPECT_TRUE(exchng->requests.empty());
  exchng->CollectRequests();
  EXPECT_EQ(exchng->requests.size(), 1);

  RequestPortfolio<Material>& rp =
      const_cast<RequestPortfolio<Material>&>(*exchng->requests.begin());
  const Request<Material>& r = *rp.requests().begin();

  EXPECT_EQ(req.commodity, r.commodity);
  EXPECT_EQ(req.target, r.target);
  EXPECT_EQ(req.preference, r.preference);
  EXPECT_EQ(req.requester, r.requester);
  EXPECT_EQ(req, r);

  clone->Decommission();
  delete fac;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, Nreq) {
  int nMats = 5;
  fac = new Requester(tc.get(), commod, pref, mat, nMats);
  Requester* clone = dynamic_cast<Requester*>(fac->Clone());
  clone->Deploy(clone);

  Request<Material> req;
  req.commodity = commod;
  req.target = mat;
  req.preference = pref;
  req.requester = clone;
  
  EXPECT_TRUE(exchng->requests.empty());
  exchng->CollectRequests();
  EXPECT_EQ(exchng->requests.size(), 1);

  RequestPortfolio<Material>& rp =
      const_cast<RequestPortfolio<Material>&>(*exchng->requests.begin());
  EXPECT_EQ(rp.requests().size(), nMats);
  
  std::vector< Request<Material> >::const_iterator it;
  const std::vector< Request<Material> >& vr = rp.requests();
  for (it = vr.begin(); it != vr.end(); ++it) {
    const Request<Material>& r = *it;  
    EXPECT_EQ(req.commodity, r.commodity);
    EXPECT_EQ(req.target, r.target);
    EXPECT_EQ(req.preference, r.preference);
    EXPECT_EQ(req.requester, r.requester);
    EXPECT_EQ(req, r);
  }
  
  clone->Decommission();
  delete fac;
}
