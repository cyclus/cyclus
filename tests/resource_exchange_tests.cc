#include <set>
#include <string>
#include <math.h>

#include <gtest/gtest.h>

#include "agent.h"
#include "bid.h"
#include "bid_portfolio.h"
#include "composition.h"
#include "equality_helpers.h"
#include "exchange_context.h"
#include "facility.h"
#include "material.h"
#include "request.h"
#include "request_portfolio.h"
#include "resource_exchange.h"
#include "resource_helpers.h"
#include "test_context.h"
#include "test_agents/test_facility.h"
using std::set;
using std::string;

namespace cyclus {

class Requester: public TestFacility {
 public:
  Requester(Context* ctx, int i = 1)
      : TestFacility(ctx),
        i_(i),
        req_ctr_(0),
        pref_ctr_(0) {}

  virtual cyclus::Agent* Clone() {
    Requester* m = new Requester(context());
    m->InitFrom(this);
    m->i_ = i_;
    m->port_ = port_;
    return m;
  }

  set<RequestPortfolio<Material>::Ptr> GetMatlRequests() {
    set<RequestPortfolio<Material>::Ptr> rps;
    RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
    rps.insert(port_);
    req_ctr_++;
    return rps;
  }

  // increments counter and squares all preferences
  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs) {
    std::map<Request<Material>*,
             std::map<Bid<Material>*, double> >::iterator p_it;
    for (p_it = prefs.begin(); p_it != prefs.end(); ++p_it) {
      std::map<Bid<Material>*, double>& map = p_it->second;
      std::map<Bid<Material>*, double>::iterator m_it;
      for (m_it = map.begin(); m_it != map.end(); ++m_it) {
        m_it->second = std::pow(m_it->second, 2);
      }
    }
    pref_ctr_++;
  }

  virtual double AdjustMatlPref(Request<Material>* req, Bid<Material>* bid,
                                double pref, TradeSense sense) {
    pref_ctr_++;
    return std::pow(pref, 2);
  }
  
  RequestPortfolio<Material>::Ptr port_;
  int i_;
  int pref_ctr_;
  int req_ctr_;
};

class Bidder: public TestFacility {
 public:
  Bidder(Context* ctx, std::string commod)
      : TestFacility(ctx),
        commod_(commod),
        bid_ctr_(0),
        pref_ctr_(0) {}

  virtual cyclus::Agent* Clone() {
    Bidder* m = new Bidder(context(), commod_);
    m->InitFrom(this);
    m->port_ = port_;
    return m;
  }

  set<BidPortfolio<Material>::Ptr> GetMatlBids(
      CommodMap<Material>::type& commod_requests) {
    set<BidPortfolio<Material>::Ptr> bps;
    bps.insert(port_);
    bid_ctr_++;
    return bps;
  }

  virtual double AdjustMatlPref(Request<Material>* req, Bid<Material>* bid,
                                double pref, TradeSense sense) {
    pref_ctr_++;
    return pref;
  }

  BidPortfolio<Material>::Ptr port_;
  std::string commod_;
  int bid_ctr_;
  int pref_ctr_;
};

class ResourceExchangeTests: public ::testing::Test {
 protected:
  TestContext tc;
  Requester* reqr;
  Bidder* bidr;
  ResourceExchange<Material>* exchng;
  string commod;
  double pref;
  Material::Ptr mat;
  Request<Material>* req;
  Bid<Material>* bid;

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
  }

  virtual void TearDown() {
    delete exchng;
  }
};

TEST_F(ResourceExchangeTests, Requests) {
  RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
  req = rp->AddRequest(mat, reqr, commod, pref);
  reqr->port_ = rp;

  Facility* clone = dynamic_cast<Facility*>(reqr->Clone());
  clone->Build(NULL);
  Requester* rcast = dynamic_cast<Requester*>(clone);
  EXPECT_EQ(0, rcast->req_ctr_);
  exchng->AddAllRequests();
  EXPECT_EQ(1, rcast->req_ctr_);
  EXPECT_EQ(1, exchng->ex_ctx().requesters.size());

  ExchangeContext<Material>& ctx = exchng->ex_ctx();

  const std::vector<RequestPortfolio<Material>::Ptr>& obsvp = ctx.requests;
  EXPECT_EQ(1, obsvp.size());
  EXPECT_TRUE(RPEq(*rp.get(), *obsvp[0].get()));

  const std::vector<Request<Material>*>& obsvr = ctx.commod_requests[commod];
  EXPECT_EQ(1, obsvr.size());
  std::vector<Request<Material>*> vr;
  vr.push_back(req);
  EXPECT_EQ(vr, obsvr);

  clone->Decommission();
}

TEST_F(ResourceExchangeTests, Bids) {
  ExchangeContext<Material>& ctx = exchng->ex_ctx();

  RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
  req = rp->AddRequest(mat, reqr, commod, pref);
  Request<Material>* req1 = rp->AddRequest(mat, reqr, commod, pref);
  ctx.AddRequestPortfolio(rp);
  const std::vector<Request<Material>*>& reqs = ctx.commod_requests[commod];
  EXPECT_EQ(2, reqs.size());

  Bidder* bidr = new Bidder(tc.get(), commod);

  BidPortfolio<Material>::Ptr bp(new BidPortfolio<Material>());
  bid = bp->AddBid(req, mat, bidr);
  Bid<Material>* bid1 = bp->AddBid(req1, mat, bidr);

  std::vector<Bid<Material>*> bids;
  bids.push_back(bid);
  bids.push_back(bid1);

  bidr->port_ = bp;

  Facility* clone = dynamic_cast<Facility*>(bidr->Clone());
  clone->Build(NULL);
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

  const std::vector<Bid<Material>*>& obsvb = ctx.bids_by_request[req];
  EXPECT_EQ(1, obsvb.size());
  std::vector<Bid<Material>*> vb;
  vb.push_back(bid);
  EXPECT_EQ(vb, obsvb);

  const std::vector<Bid<Material>*>& obsvb1 = ctx.bids_by_request[req1];
  EXPECT_EQ(1, obsvb1.size());
  vb.clear();
  vb.push_back(bid1);
  EXPECT_EQ(vb, obsvb1);

  clone->Decommission();
}

TEST_F(ResourceExchangeTests, PrefAdjustment) {
  Facility* rparent = dynamic_cast<Facility*>(reqr->Clone());
  Facility* rchild = dynamic_cast<Facility*>(reqr->Clone());
  rparent->Build(NULL);
  rchild->Build(rparent);
  Requester* rpcast = dynamic_cast<Requester*>(rparent);
  Requester* rccast = dynamic_cast<Requester*>(rchild);

  Bidder* bidr = new Bidder(tc.get(), commod);
  Facility* bparent = dynamic_cast<Facility*>(bidr->Clone());
  Facility* bchild = dynamic_cast<Facility*>(bidr->Clone());
  bparent->Build(NULL);
  bchild->Build(bparent);
  Bidder* bpcast = dynamic_cast<Bidder*>(bparent);
  Bidder* bccast = dynamic_cast<Bidder*>(bchild);
  
  // doin a little magic to simulate each requester making their own request
  RequestPortfolio<Material>::Ptr rp1(new RequestPortfolio<Material>());
  Request<Material>* preq = rp1->AddRequest(mat, rpcast, commod, pref);
  rpcast->port_ = rp1;
  RequestPortfolio<Material>::Ptr rp2(new RequestPortfolio<Material>());
  Request<Material>* creq = rp2->AddRequest(mat, rccast, commod, pref);
  rccast->port_ = rp2;

  BidPortfolio<Material>::Ptr bp(new BidPortfolio<Material>());
  Bid<Material>* pbid = bp->AddBid(preq, mat, bccast);
  Bid<Material>* cbid = bp->AddBid(creq, mat, bccast);
  bccast->port_ = bp;
  BidPortfolio<Material>::Ptr foo(new BidPortfolio<Material>());
  bpcast->port_ = foo;
  
  EXPECT_EQ(0, rpcast->req_ctr_);
  EXPECT_EQ(0, rccast->req_ctr_);
  EXPECT_NO_THROW(exchng->AddAllRequests());
  EXPECT_EQ(2, exchng->ex_ctx().requesters.size());
  EXPECT_EQ(1, rpcast->req_ctr_);
  EXPECT_EQ(1, rccast->req_ctr_);
  EXPECT_NO_THROW(exchng->AddAllBids());

  PrefMap<Material>::type pobs;
  pobs[preq].insert(std::make_pair(pbid, preq->preference()));
  PrefMap<Material>::type cobs;
  cobs[creq].insert(std::make_pair(cbid, creq->preference()));

  ExchangeContext<Material>& context = exchng->ex_ctx();
  EXPECT_EQ(context.trader_prefs[rparent], pobs);
  EXPECT_EQ(context.trader_prefs[rchild], cobs);

  EXPECT_EQ(0, rpcast->pref_ctr_);
  EXPECT_EQ(0, rccast->pref_ctr_);
  EXPECT_EQ(0, bccast->pref_ctr_);
  EXPECT_EQ(0, bpcast->pref_ctr_);
  EXPECT_NO_THROW(exchng->AdjustAll());
  // rchild gets to adjust once - its own request
  // rparent gets to adjust once for each possible trade
  EXPECT_EQ(2, rpcast->pref_ctr_);
  EXPECT_EQ(1, rccast->pref_ctr_);
  // bchild gets no adjustment
  // bparent gets to adjust once for each possible trade
  EXPECT_EQ(2, bpcast->pref_ctr_);
  EXPECT_EQ(0, bccast->pref_ctr_);
  pobs[preq].begin()->second = std::pow(preq->preference(), 2);
  cobs[creq].begin()->second = std::pow(std::pow(creq->preference(), 2), 2);
  EXPECT_EQ(context.trader_prefs[rparent], pobs);
  EXPECT_EQ(context.trader_prefs[rchild], cobs);

  rchild->Decommission();
  rparent->Decommission();
  bchild->Decommission();
  bparent->Decommission();
}

}
