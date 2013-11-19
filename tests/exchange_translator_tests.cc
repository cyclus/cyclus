#include <gtest/gtest.h>

#include "exchange_translator.h"
#include "resource.h"
#include "material.h"
#include "composition.h"
#include "bid.h"
#include "request.h"
#include "trade.h"
#include "exchange_graph.h"
#include "exchange_context.h"
#include "capacity_constraint.h"
#include "request_portfolio.h"
#include "bid_portfolio.h"
#include "test_context.h"
#include "mock_facility.h"
#include "resource_helpers.h"

using cyclus::TestContext;
using cyclus::Resource;
using cyclus::Material;
using cyclus::Request;
using cyclus::Bid;
using cyclus::RequestPortfolio;
using cyclus::BidPortfolio;
using cyclus::Match;
using cyclus::Arc;
using cyclus::Node;
using cyclus::Trade;
using cyclus::ExchangeContext;
using cyclus::ExchangeTranslator;
using cyclus::CompMap;
using cyclus::Composition;
using cyclus::CapacityConstraint;
using cyclus::TranslateCapacities;
using cyclus::RequestSet;
using cyclus::NodeSet;
using cyclus::ExchangeGraph;

// ----- xlate helpers ------
double fraction = 0.7;
int u235 = 92235;
double qty = 6.3;

double converter1(Material::Ptr m) {
  const CompMap& comp = m->comp()->mass();
  double uamt = comp.find(u235)->second;
  return comp.find(u235)->second * fraction;
}

double converter2(Material::Ptr m) {
  const CompMap& comp = m->comp()->mass();
  double uamt = comp.find(u235)->second;
  return comp.find(u235)->second * fraction * fraction;
}
// ----- xlate helpers ------

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, XlateCapacities) {
  Material::Ptr mat = get_mat(u235, qty);
  
  double qty1 = 2.5 * qty;
  CapacityConstraint<Material> cc1;
  cc1.capacity = qty1;
  cc1.converter = &converter1;

  double qty2 = 0.8 * qty;
  CapacityConstraint<Material> cc2;
  cc2.capacity = qty2;
  cc2.converter = &converter2;
  
  CapacityConstraint<Material> carr1[] = {cc1, cc2};
  std::set< CapacityConstraint<Material> >
      rconstrs(carr1, carr1 + sizeof(carr1) / sizeof(carr1[0]));

  CapacityConstraint<Material> carr2[] = {cc1};
  std::set< CapacityConstraint<Material> >
      bconstrs(carr2, carr2 + sizeof(carr2) / sizeof(carr2[0]));

  Node::Ptr rnode(new Node());
  Node::Ptr bnode(new Node());
  Arc::Ptr arc(new Arc(rnode, bnode));

  double rarr[] = {(converter1(mat) / qty1), (converter2(mat) / qty2)};
  std::vector<double> rexp(rarr, rarr +sizeof(rarr) / sizeof(rarr[0]));
      
  double barr[] = {(converter1(mat) / qty1)};
  std::vector<double> bexp(barr, barr +sizeof(barr) / sizeof(barr[0]));
      
  TranslateCapacities<Material>(mat, rconstrs, rnode, arc);
  EXPECT_EQ(rexp, rnode->unit_capacities[arc.get()]);

  TranslateCapacities<Material>(mat, bconstrs, bnode, arc);
  EXPECT_EQ(bexp, bnode->unit_capacities[arc.get()]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, XlateReq) {
  Material::Ptr mat = get_mat(u235, qty);
  
  Request<Material>::Ptr req(new Request<Material>());
  req->target = mat;
  double qty1 = 2.5 * qty;

  CapacityConstraint<Material> cc1;
  cc1.capacity = qty1;
  cc1.converter = &converter1;

  double qty2 = 0.8 * qty;
  CapacityConstraint<Material> cc2;
  cc2.capacity = qty2;
  cc2.converter = &converter2;
  
  double carr[] = {qty1, qty2};
  std::vector<double> cexp(carr, carr + sizeof(carr) / sizeof(carr[0]));
  
  RequestPortfolio<Material> rp;
  rp.AddRequest(req);
  rp.AddConstraint(cc1);
  rp.AddConstraint(cc2);

  ExchangeContext<Material> ctx;
  ExchangeTranslator<Material> xlator(&ctx);

  RequestSet::Ptr set = xlator.__TranslateRequestPortfolio(rp);

  EXPECT_EQ(qty, set->qty);
  EXPECT_EQ(cexp, set->capacities);
  EXPECT_TRUE(xlator.request_to_node_.find(req)
              != xlator.request_to_node_.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, XlateBid) {
  Material::Ptr mat = get_mat(u235, qty);
  
  Request<Material>::Ptr req(new Request<Material>());
  Bid<Material>::Ptr bid(new Bid<Material>());
  bid->offer = mat;
  bid->request = req;
  
  double qty1 = 2.5 * qty;
  CapacityConstraint<Material> cc1;
  cc1.capacity = qty1;
  cc1.converter = &converter1;

  double qty2 = 0.8 * qty;
  CapacityConstraint<Material> cc2;
  cc2.capacity = qty2;
  cc2.converter = &converter2;
  
  double carr[] = {qty1, qty2};
  std::vector<double> cexp(carr, carr + sizeof(carr) / sizeof(carr[0]));
  
  BidPortfolio<Material> port;
  port.AddBid(bid);
  port.AddConstraint(cc1);
  port.AddConstraint(cc2);

  ExchangeContext<Material> ctx;
  ExchangeTranslator<Material> xlator(&ctx);

  NodeSet::Ptr set = xlator.__TranslateBidPortfolio(port);

  EXPECT_EQ(cexp, set->capacities);
  EXPECT_TRUE(xlator.bid_to_node_.find(bid)
              != xlator.bid_to_node_.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, XlateArc) {
  Material::Ptr mat = get_mat(u235, qty);
  
  Request<Material>::Ptr req(new Request<Material>());
  req->target = mat;
  
  Bid<Material>::Ptr bid(new Bid<Material>());
  bid->offer = mat;
  bid->request = req;
  
  double qty1 = 2.5 * qty;
  CapacityConstraint<Material> cc1;
  cc1.capacity = qty1;
  cc1.converter = &converter1;

  double qty2 = 0.8 * qty;
  CapacityConstraint<Material> cc2;
  cc2.capacity = qty2;
  cc2.converter = &converter2;
  
  double carr[] = {qty1, qty2};
  std::vector<double> cexp(carr, carr + sizeof(carr) / sizeof(carr[0]));
  
  BidPortfolio<Material> bport;
  bport.AddBid(bid);
  bport.AddConstraint(cc1);
  bport.AddConstraint(cc2);

  RequestPortfolio<Material> rport;
  rport.AddRequest(req);
  rport.AddConstraint(cc1);

  ExchangeContext<Material> ctx;
  ExchangeTranslator<Material> xlator(&ctx);

  // give the xlator the correct state
  RequestSet::Ptr rset = xlator.__TranslateRequestPortfolio(rport);
  NodeSet::Ptr bset = xlator.__TranslateBidPortfolio(bport);

  Arc::Ptr a = xlator.__TranslateArc(bid);

  EXPECT_EQ(xlator.bid_to_node_[bid], a->vnode);
  EXPECT_EQ(xlator.request_to_node_[req], a->unode);

  double barr[] = {(converter1(mat) / qty1), (converter2(mat) / qty2)};
  std::vector<double> bexp(barr, barr +sizeof(barr) / sizeof(barr[0]));
  EXPECT_EQ(bexp, a->vnode->unit_capacities[a.get()]);
      
  double rarr[] = {(converter1(mat) / qty1)};
  std::vector<double> rexp(rarr, rarr +sizeof(rarr) / sizeof(rarr[0]));
  EXPECT_EQ(rexp, a->unode->unit_capacities[a.get()]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, NullXlate) {
  TestContext tc;
  MockFacility* fac = new MockFacility(tc.get());
  Material::Ptr mat = get_mat(u235, qty);
  
  Request<Material>::Ptr req(new Request<Material>());
  req->target = mat;
  req->requester = fac;
  
  Bid<Material>::Ptr bid(new Bid<Material>());
  bid->offer = mat;
  bid->request = req;
  bid->bidder = fac;
  
  BidPortfolio<Material> bport;
  bport.AddBid(bid);

  RequestPortfolio<Material> rport;
  rport.AddRequest(req);

  ExchangeContext<Material> ctx;
  ctx.AddRequestPortfolio(rport);
  ctx.AddBidPortfolio(bport);
  
  ExchangeTranslator<Material> xlator(&ctx);
  
  ExchangeGraph::Ptr graph;
  EXPECT_NO_THROW(graph = xlator.Translate());
  EXPECT_EQ(1, graph->request_sets.size());
  EXPECT_EQ(1, graph->supply_sets.size());
  EXPECT_EQ(2, graph->node_arc_map.size());
  EXPECT_EQ(1, graph->arcs_.size());
  EXPECT_EQ(0, graph->matches.size());

  delete fac;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, BackXlate) {
  ExchangeContext<Resource> ctx;
  ExchangeTranslator<Resource> xlator(&ctx);
  
  Request<Resource>::Ptr ur(new Request<Resource>());
  Request<Resource>::Ptr xr(new Request<Resource>());
  Bid<Resource>::Ptr vb(new Bid<Resource>());
  Bid<Resource>::Ptr yb(new Bid<Resource>());
  
  Node::Ptr u(new Node());  
  Node::Ptr v(new Node());
  Node::Ptr x(new Node());  
  Node::Ptr y(new Node());

  xlator.__AddRequest(ur, u);
  xlator.__AddRequest(xr, x);
  xlator.__AddBid(vb, v);
  xlator.__AddBid(yb, y);
  
  Arc::Ptr a(new Arc(u, v));
  Arc::Ptr b(new Arc(x, y));

  double qty = 2.5; // some magic numbers
  double aqty = qty * 0.1;
  double bqty = qty * 1.5;

  Trade<Resource> aexp(ur, vb, aqty);
  Trade<Resource> bexp(xr, yb, bqty);
  
  Trade<Resource> tarr[] = {aexp, bexp};
  std::vector< Trade<Resource> > exp(tarr, tarr + sizeof(tarr) / sizeof(tarr[0]));
  
  Match amatch(std::make_pair(a, aqty));
  Match bmatch(std::make_pair(b, bqty));
  
  Match marr[] = {amatch, bmatch};
  std::vector<Match> matches(marr, marr + sizeof(marr) / sizeof(marr[0]));

  std::vector< Trade<Resource> > obs = xlator.BackTranslateSolution(matches);
  EXPECT_EQ(exp, obs);
}
