
#include <gtest/gtest.h>

#include <set>
#include <string>

#include "resource_exchange.h"

#include "composition.h"
#include "test_context.h"
#include "request.h"
#include "request_portfolio.h"
#include "bid.h"
#include "bid_portfolio.h"
#include "mock_facility.h"
#include "material.h"
#include "model.h"
#include "facility_model.h"
#include "exchange_context.h"

using std::set;
using std::string;
using cyclus::Composition;
using cyclus::Context;
using cyclus::ExchangeContext;
using cyclus::FacilityModel;
using cyclus::Material;
using cyclus::Model;
using cyclus::TestContext;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::Bid;
using cyclus::BidPortfolio;
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

class Bidder: public MockFacility {
 public:
  Bidder(Context* ctx, std::vector<Bid<Material>::Ptr> bids, std::string commod)
      : MockFacility(ctx),
        Model(ctx),
        bids_(bids),
        commod_(commod)
  { };

  virtual cyclus::Model* Clone() {
    Bidder* m = new Bidder(*this);
    m->InitFrom(this);
    m->bids_ = bids_;
    m->commod_ = commod_;
    return m;
  };
  
  set< BidPortfolio<Material> > AddMatlBids(ExchangeContext<Material>* ec) {
    set< BidPortfolio<Material> > bps;
    BidPortfolio<Material> bp;
    int sz = ec->RequestsForCommod(commod_).size();
    for (int i = 0; i < sz; i++) {
      bp.AddBid(bids_[i]);
    }
    bps.insert(bp);
    return bps;
  }

  std::vector<Bid<Material>::Ptr> bids_;
  std::string commod_;
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
  Bid<Material>::Ptr bid;
  
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
    
    bid = Bid<Material>::Ptr(new Bid<Material>());
    bid->request = req;
    bid->offer = mat;

    exchng = new ResourceExchange<Material>(tc.get());
  };
  
  virtual void TearDown() {
    delete exchng;
  };
  
};

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
TEST_F(ResourceExchangeTests, requests) {
  fac = new Requester(tc.get(), req);

  FacilityModel* clone = dynamic_cast<FacilityModel*>(fac->Clone());
  clone->Deploy(clone);

  exchng->CollectRequests();

  const ExchangeContext<Material>& ctx = exchng->const_ex_ctx();
  
  std::vector<RequestPortfolio<Material> > vp;
  RequestPortfolio<Material> rp;
  rp.AddRequest(req);
  vp.push_back(rp);
  const std::vector< RequestPortfolio<Material> >& obsvp = ctx.requests();
  EXPECT_EQ(vp, obsvp);

  const std::vector<Request<Material>::Ptr>& obsvr = ctx.RequestsForCommod(commod);
  EXPECT_EQ(1, obsvr.size());  
  std::vector<Request<Material>::Ptr> vr;
  vr.push_back(req);
  EXPECT_EQ(vr, obsvr);
  
  clone->Decommission();
  delete fac;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, bids) {
  ExchangeContext<Material>& ctx = exchng->ex_ctx();
  
  RequestPortfolio<Material> rp;
  Request<Material>::Ptr req1 = Request<Material>::Ptr(new Request<Material>);
  req1->commodity = commod;
  req1->preference = pref;
  req1->target = mat;
  
  rp.AddRequest(req);
  rp.AddRequest(req1);
  ctx.AddRequestPortfolio(rp);
  const std::vector<Request<Material>::Ptr>& reqs = ctx.RequestsForCommod(commod);
  EXPECT_EQ(2, reqs.size());
  

  Bid<Material>::Ptr bid1 = Bid<Material>::Ptr(new Bid<Material>());
  bid1->request = req1;
  bid->offer = mat;
  
  std::vector<Bid<Material>::Ptr> bids;
  bids.push_back(bid);
  bids.push_back(bid1);
  
  Bidder* bidr = new Bidder(tc.get(), bids, commod);

  FacilityModel* clone = dynamic_cast<FacilityModel*>(bidr->Clone());
  clone->Deploy(clone);

  exchng->CollectBids();
  
  std::vector<BidPortfolio<Material> > vp;
  BidPortfolio<Material> bp;
  bp.AddBid(bid);
  bp.AddBid(bid1);
  vp.push_back(bp);
  const std::vector< BidPortfolio<Material> >& obsvp = ctx.bids();
  EXPECT_EQ(vp, obsvp);

  const std::vector<Bid<Material>::Ptr>& obsvb = ctx.BidsForRequest(req);
  EXPECT_EQ(1, obsvb.size());  
  std::vector<Bid<Material>::Ptr> vb;
  vb.push_back(bid);
  EXPECT_EQ(vb, obsvb);

  const std::vector<Bid<Material>::Ptr>& obsvb1 = ctx.BidsForRequest(req1);
  EXPECT_EQ(1, obsvb1.size());  
  vb.clear();
  vb.push_back(bid1);
  EXPECT_EQ(vb, obsvb1);
  
  clone->Decommission();
  delete bidr;
}
