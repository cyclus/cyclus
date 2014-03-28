#include <gtest/gtest.h>

#include <set>
#include <string>
#include <math.h>

#include "bid.h"
#include "bid_portfolio.h"
#include "composition.h"
#include "equality_helpers.h"
#include "exchange_context.h"
#include "facility.h"
#include "material.h"
#include "test_modules/test_facility.h"
#include "agent.h"
#include "request.h"
#include "request_portfolio.h"
#include "resource_helpers.h"
#include "test_context.h"

#include "resource_exchange.h"

using cyclus::Bid;
using cyclus::BidPortfolio;
using cyclus::CommodMap;
using cyclus::Composition;
using cyclus::Context;
using cyclus::ExchangeContext;
using cyclus::Facility;
using cyclus::Material;
using cyclus::Agent;
using cyclus::PrefMap;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::ResourceExchange;
using cyclus::TestContext;
using std::set;
using std::string;
using rsrc_helpers::get_req;
using rsrc_helpers::trader;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class Requester: public TestFacility {
 public:
  Requester(Context* ctx, int i = 1)
      : TestFacility(ctx),
        i_(i),
        req_ctr_(0),
        pref_ctr_(0)
  {};

  virtual cyclus::Agent* Clone() {
    Requester* m = new Requester(context());
    m->InitFrom(this);
    m->i_ = i_;
    m->port_ = port_;
    return m;
  };
  
  set<RequestPortfolio<Material>::Ptr> GetMatlRequests() {
    set<RequestPortfolio<Material>::Ptr> rps;
    RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
    rps.insert(port_);
    req_ctr_++;
    return rps;
  }

  // increments counter and squares all preferences
  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs) {
    std::map<Request<Material>::Ptr,
             std::map<Bid<Material>::Ptr, double> >::iterator p_it;  
    for (p_it = prefs.begin(); p_it != prefs.end(); ++p_it) {
      std::map<Bid<Material>::Ptr, double>& map = p_it->second;
      std::map<Bid<Material>::Ptr, double>::iterator m_it;
      for (m_it = map.begin(); m_it != map.end(); ++m_it) {
        m_it->second = std::pow(m_it->second, 2);
      }
    }
    pref_ctr_++;
  }
  
  RequestPortfolio<Material>::Ptr port_;
  int i_;
  int pref_ctr_;
  int req_ctr_;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class Bidder: public TestFacility {
 public:
  Bidder(Context* ctx, std::string commod)
      : TestFacility(ctx),
        commod_(commod),
        bid_ctr_(0)
  {};

  virtual cyclus::Agent* Clone() {
    Bidder* m = new Bidder(context(), commod_);
    m->InitFrom(this);
    m->port_ = port_;
    return m;
  };
  
  set<BidPortfolio<Material>::Ptr> GetMatlBids(
      const CommodMap<Material>::type& commod_requests) {
    set<BidPortfolio<Material>::Ptr> bps;
    bps.insert(port_);
    bid_ctr_++;
    return bps;
  }

  BidPortfolio<Material>::Ptr port_;
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
    exchng = new ResourceExchange<Material>(tc.get());
  };
  
  virtual void TearDown() {
    delete exchng;
  };
  
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, Requests) {
  RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
  req = rp->AddRequest(mat, reqr, commod, pref);
  reqr->port_ = rp;
  
  Facility* clone = dynamic_cast<Facility*>(reqr->Clone());
  clone->Build();
  Requester* rcast = dynamic_cast<Requester*>(clone);
  EXPECT_EQ(0, rcast->req_ctr_);
  exchng->AddAllRequests();
  EXPECT_EQ(1, rcast->req_ctr_);
  EXPECT_EQ(1, exchng->ex_ctx().requesters.size());
  
  ExchangeContext<Material>& ctx = exchng->ex_ctx();
  
  const std::vector<RequestPortfolio<Material>::Ptr>& obsvp = ctx.requests;
  EXPECT_EQ(1, obsvp.size());
  EXPECT_TRUE(RPEq(*rp.get(), *obsvp[0].get()));

  const std::vector<Request<Material>::Ptr>& obsvr = ctx.commod_requests[commod];
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
  req = rp->AddRequest(mat, reqr, commod, pref);
  Request<Material>::Ptr req1 = rp->AddRequest(mat, reqr, commod, pref);
  ctx.AddRequestPortfolio(rp);
  const std::vector<Request<Material>::Ptr>& reqs = ctx.commod_requests[commod];
  EXPECT_EQ(2, reqs.size());  
  
  Bidder* bidr = new Bidder(tc.get(), commod);

  BidPortfolio<Material>::Ptr bp(new BidPortfolio<Material>());
  bid = bp->AddBid(req, mat, bidr);
  Bid<Material>::Ptr bid1 = bp->AddBid(req1, mat, bidr);
  
  std::vector<Bid<Material>::Ptr> bids;
  bids.push_back(bid);
  bids.push_back(bid1);

  bidr->port_ = bp;
  
  Facility* clone = dynamic_cast<Facility*>(bidr->Clone());
  clone->Build();
  Bidder* bcast = dynamic_cast<Bidder*>(clone);

  EXPECT_EQ(0, bcast->bid_ctr_);
  exchng->AddAllBids();
  EXPECT_EQ(1, bcast->bid_ctr_);
  EXPECT_EQ(1, exchng->ex_ctx().bidders.size());
  
  const std::vector<BidPortfolio<Material>::Ptr>& obsvp = ctx.bids;
  EXPECT_EQ(1, obsvp.size());
  EXPECT_TRUE(BPEq(*bp.get(), *obsvp[0].get()));
  const cyclus::BidPortfolio<Material>& lhs = *bp;
  const cyclus::BidPortfolio<Material>& rhs = *obsvp[0];
  EXPECT_TRUE(BPEq(*bp, *obsvp[0]));

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
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, PrefCalls) {
  Facility* parent = dynamic_cast<Facility*>(reqr->Clone());
  Facility* child = dynamic_cast<Facility*>(reqr->Clone());
  parent->Build();
  child->Build(parent);
    
  Requester* pcast = dynamic_cast<Requester*>(parent);
  Requester* ccast = dynamic_cast<Requester*>(child);

  ASSERT_TRUE(pcast != NULL);
  ASSERT_TRUE(ccast != NULL);
  ASSERT_TRUE(pcast->parent() == NULL);
  ASSERT_TRUE(ccast->parent() == dynamic_cast<Agent*>(pcast));
  ASSERT_TRUE(pcast->manager() == dynamic_cast<Agent*>(pcast));
  ASSERT_TRUE(ccast->manager() == dynamic_cast<Agent*>(ccast));

  // doin a little magic to simulate each requester making their own request
  RequestPortfolio<Material>::Ptr rp1(new RequestPortfolio<Material>());
  Request<Material>::Ptr preq = rp1->AddRequest(mat, pcast, commod, pref);
  pcast->port_ = rp1;
  RequestPortfolio<Material>::Ptr rp2(new RequestPortfolio<Material>());
  Request<Material>::Ptr creq = rp2->AddRequest(mat, ccast, commod, pref);
  ccast->port_ = rp2;
    
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
  Facility* parent = dynamic_cast<Facility*>(reqr->Clone());
  Facility* child = dynamic_cast<Facility*>(reqr->Clone());
  parent->Build();
  child->Build(parent);
    
  Requester* pcast = dynamic_cast<Requester*>(parent);
  Requester* ccast = dynamic_cast<Requester*>(child);

  // doin a little magic to simulate each requester making their own request
  RequestPortfolio<Material>::Ptr rp1(new RequestPortfolio<Material>());
  Request<Material>::Ptr preq = rp1->AddRequest(mat, pcast, commod, pref);
  pcast->port_ = rp1;
  RequestPortfolio<Material>::Ptr rp2(new RequestPortfolio<Material>());
  Request<Material>::Ptr creq = rp2->AddRequest(mat, ccast, commod, pref);
  ccast->port_ = rp2;
    
  Bidder* bidr = new Bidder(tc.get(), commod);
  
  BidPortfolio<Material>::Ptr bp(new BidPortfolio<Material>());
  Bid<Material>::Ptr pbid = bp->AddBid(preq, mat, bidr);
  Bid<Material>::Ptr cbid = bp->AddBid(creq, mat, bidr);
  
  std::vector<Bid<Material>::Ptr> bids;
  bids.push_back(pbid);
  bids.push_back(cbid);
  bidr->port_ = bp;
  
  Facility* bclone = dynamic_cast<Facility*>(bidr->Clone());
  bclone->Build();
  
  EXPECT_NO_THROW(exchng->AddAllRequests());
  EXPECT_NO_THROW(exchng->AddAllBids());

  PrefMap<Material>::type pobs;
  pobs[preq].insert(std::make_pair(pbid, preq->preference()));
  PrefMap<Material>::type cobs;
  cobs[creq].insert(std::make_pair(cbid, creq->preference()));

  ExchangeContext<Material>& context = exchng->ex_ctx();  
  EXPECT_EQ(context.trader_prefs[parent], pobs);
  EXPECT_EQ(context.trader_prefs[child], cobs);
  
  EXPECT_NO_THROW(exchng->AdjustAll());

  pobs[preq].begin()->second = std::pow(preq->preference(), 2);
  cobs[creq].begin()->second = std::pow(std::pow(creq->preference(), 2), 2);
  EXPECT_EQ(context.trader_prefs[parent], pobs);
  EXPECT_EQ(context.trader_prefs[child], cobs);
  
  child->Decommission();
  parent->Decommission();
}
