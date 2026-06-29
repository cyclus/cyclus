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

using cyclus::Bid;
using cyclus::BidPortfolio;
using cyclus::CommodMap;
using cyclus::Composition;
using cyclus::Context;
using cyclus::ExchangeContext;
using cyclus::Facility;
using cyclus::Material;
using cyclus::Agent;
using cyclus::RequestBidMap;
using cyclus::RequestBidMap;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::ResourceExchange;
using cyclus::TestContext;
using std::set;
using std::string;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class Requester: public TestFacility {
 public:
  Requester(Context* ctx, int i = 1)
      : TestFacility(ctx),
        i_(i),
        req_ctr_(0),
        arc_ctr_(0) {}

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

  // increments counter and squares all arc_costs directly
virtual void AdjustMatlParams(RequestBidMap<Material>::type& rb_map) {
  for (auto& request_bids : rb_map) {
    auto& bid_map = request_bids.second;
    for (auto& bid_cost : bid_map) {
      bid_cost.second = std::pow(bid_cost.second, 2);
    }
  }
  arc_ctr_++;
}

  RequestPortfolio<Material>::Ptr port_;
  int i_;
  int arc_ctr_;
  int req_ctr_;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class Bidder: public TestFacility {
 public:
  Bidder(Context* ctx, std::string commod)
      : TestFacility(ctx),
        commod_(commod),
        bid_ctr_(0) {}

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
  double unit_value;
  Material::Ptr mat;
  Request<Material>* req;
  Bid<Material>* bid;

  virtual void SetUp() {
    commod = "name";
    unit_value = 2.4;
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, Requests) {
  RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
  req = rp->AddRequest(mat, reqr, commod, unit_value);
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, Bids) {
  ExchangeContext<Material>& ctx = exchng->ex_ctx();

  RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
  req = rp->AddRequest(mat, reqr, commod, unit_value);
  Request<Material>* req1 = rp->AddRequest(mat, reqr, commod, unit_value);
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, ArcCostCalls) {
  Facility* parent = dynamic_cast<Facility*>(reqr->Clone());
  Facility* child = dynamic_cast<Facility*>(reqr->Clone());
  parent->Build(NULL);
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
  Request<Material>* preq = rp1->AddRequest(mat, pcast, commod, unit_value);
  pcast->port_ = rp1;
  RequestPortfolio<Material>::Ptr rp2(new RequestPortfolio<Material>());
  Request<Material>* creq = rp2->AddRequest(mat, ccast, commod, unit_value);
  ccast->port_ = rp2;

  EXPECT_EQ(0, pcast->req_ctr_);
  EXPECT_EQ(0, ccast->req_ctr_);
  exchng->AddAllRequests();
  EXPECT_EQ(2, exchng->ex_ctx().requesters.size());
  EXPECT_EQ(1, pcast->req_ctr_);
  EXPECT_EQ(1, ccast->req_ctr_);

  EXPECT_EQ(0, pcast->arc_ctr_);
  EXPECT_EQ(0, ccast->arc_ctr_);
  EXPECT_NO_THROW(exchng->AdjustAll());

  // child gets to adjust once - its own request
  // parent gets called twice - its request and adjusting its child's request
  EXPECT_EQ(2, pcast->arc_ctr_);
  EXPECT_EQ(1, ccast->arc_ctr_);

  child->Decommission();
  parent->Decommission();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ResourceExchangeTests, ArcCostValues) {
  Facility* parent = dynamic_cast<Facility*>(reqr->Clone());
  Facility* child = dynamic_cast<Facility*>(reqr->Clone());
  parent->Build(NULL);
  child->Build(parent);

  Requester* pcast = dynamic_cast<Requester*>(parent);
  Requester* ccast = dynamic_cast<Requester*>(child);

  // doin a little magic to simulate each requester making their own request
  RequestPortfolio<Material>::Ptr rp1(new RequestPortfolio<Material>());
  Request<Material>* preq = rp1->AddRequest(mat, pcast, commod, unit_value);
  pcast->port_ = rp1;
  RequestPortfolio<Material>::Ptr rp2(new RequestPortfolio<Material>());
  Request<Material>* creq = rp2->AddRequest(mat, ccast, commod, unit_value);
  ccast->port_ = rp2;

  Bidder* bidr = new Bidder(tc.get(), commod);

  BidPortfolio<Material>::Ptr bp(new BidPortfolio<Material>());

  // Bids without a unit_cost default to unit_cost = 0
  Bid<Material>* pbid = bp->AddBid(preq, mat, bidr, false);
  Bid<Material>* cbid = bp->AddBid(creq, mat, bidr, false);

  std::vector<Bid<Material>*> bids;
  bids.push_back(pbid);
  bids.push_back(cbid);
  bidr->port_ = bp;

  Facility* bclone = dynamic_cast<Facility*>(bidr->Clone());
  bclone->Build(NULL);

  EXPECT_NO_THROW(exchng->AddAllRequests());
  EXPECT_NO_THROW(exchng->AddAllBids());

  double p_arc_cost = pbid->unit_cost() - preq->pref_mod();
  double c_arc_cost = cbid->unit_cost() - creq->pref_mod();

  RequestBidMap<Material>::type pexp;
  pexp[preq].insert(std::make_pair(pbid, p_arc_cost));
  RequestBidMap<Material>::type cexp;
  cexp[creq].insert(std::make_pair(cbid, c_arc_cost));

  ExchangeContext<Material>& context = exchng->ex_ctx();
  EXPECT_EQ(context.trader_arc_costs[parent], pexp);
  EXPECT_EQ(context.trader_arc_costs[child], cexp);

  EXPECT_NO_THROW(exchng->AdjustAll());

  pexp[preq].begin()->second = std::pow(p_arc_cost, 2);
  cexp[creq].begin()->second = std::pow(std::pow(c_arc_cost, 2), 2);
  EXPECT_EQ(context.trader_arc_costs[parent], pexp);
  EXPECT_EQ(context.trader_arc_costs[child], cexp);

  child->Decommission();
  parent->Decommission();
}
