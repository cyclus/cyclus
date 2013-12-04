#include <gtest/gtest.h>

#include <set>
#include <string>
#include <math.h>

#include "bid.h"
#include "bid_portfolio.h"
#include "composition.h"
#include "equality_helpers.h"
#include "exchange_context.h"
#include "facility_model.h"
#include "material.h"
#include "mock_facility.h"
#include "model.h"
#include "request.h"
#include "request_portfolio.h"
#include "resource_helpers.h"
#include "test_context.h"

#include "resource_exchange.h"

using cyclus::Bid;
using cyclus::BidPortfolio;
using cyclus::Composition;
using cyclus::Context;
using cyclus::ExchangeContext;
using cyclus::FacilityModel;
using cyclus::Material;
using cyclus::Model;
using cyclus::PrefMap;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::ResourceExchange;
using cyclus::TestContext;
using std::set;
using std::string;
using test_helpers::get_req;
using test_helpers::trader;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class Requester: public MockFacility {
 public:
  Requester(Context* ctx, int i = 1)
      : MockFacility(ctx),
        Model(ctx),
        i_(i),
        req_ctr_(0),
        pref_ctr_(0)
  {};

  virtual cyclus::Model* Clone() {
    Requester* m = new Requester(*this);
    m->InitFrom(this);
    m->r_ = r_;
    m->i_ = i_;
    return m;
  };
  
  set<RequestPortfolio<Material>::Ptr> GetMatlRequests() {
    set<RequestPortfolio<Material>::Ptr> rps;
    RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
    for (int i = 0; i < i_; i++) {
      rp->AddRequest(r_);
    }
    rps.insert(rp);
    req_ctr_++;
    return rps;
  }

  // increments counter and squares all preferences
  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs) {
    std::map<Request<Material>::Ptr,
             std::vector<std::pair<Bid<Material>::Ptr, double> > >::iterator m_it;
    for (m_it = prefs.begin(); m_it != prefs.end(); ++m_it) {
      for (int i = 0; i < m_it->second.size(); i++) {
        m_it->second[i].second = std::pow(m_it->second[i].second, 2);
      }
    }    
    pref_ctr_++;
  }
  
  Request<Material>::Ptr r_;
  int i_;
  int pref_ctr_;
  int req_ctr_;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class Bidder: public MockFacility {
 public:
  Bidder(Context* ctx, std::string commod)
      : MockFacility(ctx),
        Model(ctx),
        commod_(commod),
        bid_ctr_(0)
  {};

  virtual cyclus::Model* Clone() {
    Bidder* m = new Bidder(*this);
    m->InitFrom(this);
    m->bids_ = bids_;
    m->commod_ = commod_;
    return m;
  };
  
  set<BidPortfolio<Material>::Ptr> GetMatlBids(ExchangeContext<Material>* ec) {
    set<BidPortfolio<Material>::Ptr> bps;
    BidPortfolio<Material>::Ptr bp(new BidPortfolio<Material>());
    int sz = ec->requests_by_commod[commod_].size();
    for (int i = 0; i < sz; i++) {
      bp->AddBid(bids_[i]);
    }
    bps.insert(bp);
    bid_ctr_++;
    return bps;
  }

  std::vector<Bid<Material>::Ptr> bids_;
  std::string commod_;
  int bid_ctr_;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ResourceExchangeTests: public ::testing::Test {
 protected:
  TestContext tc;
  Requester* reqr;
  Bidder* bidr;
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

    reqr = new Requester(tc.get());
    req = Request<Material>::Ptr(
        new Request<Material>(mat, reqr, commod, pref));
    reqr->r_ = req;
    
    bid = Bid<Material>::Ptr(new Bid<Material>(req, mat, &trader));

    exchng = new ResourceExchange<Material>(tc.get());
  };
  
  virtual void TearDown() {
    delete reqr;
    delete exchng;
  };
  
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, Cloning) {
  EXPECT_EQ(req, reqr->r_);  

  FacilityModel* clone = dynamic_cast<FacilityModel*>(reqr->Clone());
  clone->Deploy(clone);

  Requester* cast = dynamic_cast<Requester*>(clone);
  EXPECT_EQ(req, cast->r_);
  
  clone->Decommission();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, Requests) {
  FacilityModel* clone = dynamic_cast<FacilityModel*>(reqr->Clone());
  clone->Deploy(clone);
  Requester* rcast = dynamic_cast<Requester*>(clone);

  EXPECT_EQ(0, rcast->req_ctr_);
  exchng->AddAllRequests();
  EXPECT_EQ(1, rcast->req_ctr_);
  EXPECT_EQ(1, exchng->ex_ctx().requesters.size());
  
  ExchangeContext<Material>& ctx = exchng->ex_ctx();
  
  RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
  rp->AddRequest(req);
  const std::vector<RequestPortfolio<Material>::Ptr>& obsvp = ctx.requests;
  EXPECT_EQ(1, obsvp.size());
  EXPECT_TRUE(RPEq(*rp.get(), *obsvp[0].get()));

  const std::vector<Request<Material>::Ptr>& obsvr = ctx.requests_by_commod[commod];
  EXPECT_EQ(1, obsvr.size());  
  std::vector<Request<Material>::Ptr> vr;
  vr.push_back(req);
  EXPECT_EQ(vr, obsvr);
  
  clone->Decommission();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, Bids) {
  ExchangeContext<Material>& ctx = exchng->ex_ctx();
  
  RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
  Request<Material>::Ptr req1(new Request<Material>(mat, reqr, commod, pref));
  
  rp->AddRequest(req);
  rp->AddRequest(req1);
  ctx.AddRequestPortfolio(rp);
  const std::vector<Request<Material>::Ptr>& reqs = ctx.requests_by_commod[commod];
  EXPECT_EQ(2, reqs.size());  
  
  Bidder* bidr = new Bidder(tc.get(), commod);

  bid = Bid<Material>::Ptr(new Bid<Material>(req, mat, bidr));
  Bid<Material>::Ptr bid1(new Bid<Material>(req1, mat, bidr));
  std::vector<Bid<Material>::Ptr> bids;
  bids.push_back(bid);
  bids.push_back(bid1);
  
  bidr->bids_ = bids;
  
  FacilityModel* clone = dynamic_cast<FacilityModel*>(bidr->Clone());
  clone->Deploy(clone);
  Bidder* bcast = dynamic_cast<Bidder*>(clone);

  EXPECT_EQ(0, bcast->bid_ctr_);
  exchng->AddAllBids();
  EXPECT_EQ(1, bcast->bid_ctr_);
  EXPECT_EQ(1, exchng->ex_ctx().bidders.size());
  
  BidPortfolio<Material>::Ptr bp(new BidPortfolio<Material>());
  bp->AddBid(bid);
  bp->AddBid(bid1);
  const std::vector<BidPortfolio<Material>::Ptr>& obsvp = ctx.bids;
  EXPECT_EQ(1, obsvp.size());
  EXPECT_TRUE(BPEq(*bp.get(), *obsvp[0].get()));

  const std::vector<Bid<Material>::Ptr>& obsvb = ctx.bids_by_request[req];
  EXPECT_EQ(1, obsvb.size());  
  std::vector<Bid<Material>::Ptr> vb;
  vb.push_back(bid);
  EXPECT_EQ(vb, obsvb);

  const std::vector<Bid<Material>::Ptr>& obsvb1 = ctx.bids_by_request[req1];
  EXPECT_EQ(1, obsvb1.size());  
  vb.clear();
  vb.push_back(bid1);
  EXPECT_EQ(vb, obsvb1);
  
  clone->Decommission();
  delete bidr;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, PrefCalls) {
  FacilityModel* parent = dynamic_cast<FacilityModel*>(reqr->Clone());
  FacilityModel* child = dynamic_cast<FacilityModel*>(reqr->Clone());
  parent->Deploy(parent);
  child->Deploy(parent);
    
  Requester* pcast = dynamic_cast<Requester*>(parent);
  Requester* ccast = dynamic_cast<Requester*>(child);

  // doin a little magic to simulate each requester making their own request
  Request<Material>::Ptr preq(new Request<Material>(mat, pcast, commod, pref));
  pcast->r_ = preq;
  Request<Material>::Ptr creq(new Request<Material>(mat, ccast, commod, pref));
  ccast->r_ = creq;
  
  EXPECT_EQ(0, pcast->req_ctr_);
  EXPECT_EQ(0, ccast->req_ctr_);
  exchng->AddAllRequests();
  EXPECT_EQ(2, exchng->ex_ctx().requesters.size());
  EXPECT_EQ(1, pcast->req_ctr_);
  EXPECT_EQ(1, ccast->req_ctr_);
  
  EXPECT_EQ(0, pcast->pref_ctr_);
  EXPECT_EQ(0, ccast->pref_ctr_);
  
  EXPECT_NO_THROW(exchng->AdjustAll());

  // child gets to adjust once - its own request
  // parent gets called twice - its request and adjusting its child's request
  EXPECT_EQ(2, pcast->pref_ctr_); 
  EXPECT_EQ(1, ccast->pref_ctr_);
  
  child->Decommission();
  parent->Decommission();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, PrefValues) {
  FacilityModel* parent = dynamic_cast<FacilityModel*>(reqr->Clone());
  FacilityModel* child = dynamic_cast<FacilityModel*>(reqr->Clone());
  parent->Deploy(parent);
  child->Deploy(parent);
    
  Requester* pcast = dynamic_cast<Requester*>(parent);
  Requester* ccast = dynamic_cast<Requester*>(child);

  // doin a little magic to simulate each requester making their own request
  Request<Material>::Ptr preq(new Request<Material>(mat, pcast, commod, pref));
  pcast->r_ = preq;
  Request<Material>::Ptr creq(new Request<Material>(mat, ccast, commod, pref));
  ccast->r_ = creq;

  Bidder* bidr = new Bidder(tc.get(), commod);

  Bid<Material>::Ptr pbid(new Bid<Material>(preq, mat, bidr));
  Bid<Material>::Ptr cbid(new Bid<Material>(creq, mat, bidr));
  
  std::vector<Bid<Material>::Ptr> bids;
  bids.push_back(pbid);
  bids.push_back(cbid);
  bidr->bids_ = bids;
  
  FacilityModel* bclone = dynamic_cast<FacilityModel*>(bidr->Clone());
  bclone->Deploy(bclone);
  
  EXPECT_NO_THROW(exchng->AddAllRequests());
  EXPECT_NO_THROW(exchng->AddAllBids());

  PrefMap<Material>::type pobs;
  pobs[preq].push_back(std::make_pair(pbid, preq->preference()));
  PrefMap<Material>::type cobs;
  cobs[creq].push_back(std::make_pair(cbid, creq->preference()));

  ExchangeContext<Material>& context = exchng->ex_ctx();  
  EXPECT_EQ(context.trader_prefs[parent], pobs);
  EXPECT_EQ(context.trader_prefs[child], cobs);
  
  EXPECT_NO_THROW(exchng->AdjustAll());

  pobs[preq][0].second = std::pow(preq->preference(), 2);
  cobs[creq][0].second = std::pow(std::pow(creq->preference(), 2), 2);
  EXPECT_EQ(context.trader_prefs[parent], pobs);
  EXPECT_EQ(context.trader_prefs[child], cobs);
  
  child->Decommission();
  parent->Decommission();
}
