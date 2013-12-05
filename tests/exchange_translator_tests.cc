#include <gtest/gtest.h>

#include "bid.h"
#include "bid_portfolio.h"
#include "capacity_constraint.h"
#include "composition.h"
#include "exchange_context.h"
#include "exchange_graph.h"
#include "exchange_translator.h"
#include "exchange_translation_context.h"
#include "material.h"
#include "mock_facility.h"
#include "request.h"
#include "request_portfolio.h"
#include "resource.h"
#include "resource_helpers.h"
#include "test_context.h"
#include "trade.h"

using cyclus::Arc;
using cyclus::Bid;
using cyclus::BidPortfolio;
using cyclus::CapacityConstraint;
using cyclus::CompMap;
using cyclus::Composition;
using cyclus::Converter;
using cyclus::ExchangeContext;
using cyclus::ExchangeGraph;
using cyclus::ExchangeTranslator;
using cyclus::Match;
using cyclus::Material;
using cyclus::ExchangeNode;
using cyclus::ExchangeNodeGroup;
using cyclus::Request;
using cyclus::RequestPortfolio;
using cyclus::RequestGroup;
using cyclus::Resource;
using cyclus::TestContext;
using cyclus::Trade;
using cyclus::TranslateCapacities;
using test_helpers::get_bid;
using test_helpers::get_mat;
using test_helpers::get_req;
using test_helpers::trader;

double fraction = 0.7;
int u235 = 92235;
double qty = 6.3;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct MatConverter1 : public Converter<Material> {
  MatConverter1() {}
  virtual ~MatConverter1() {}
  
  virtual double convert(Material::Ptr r) {
    const CompMap& comp = r->comp()->mass();
    double uamt = comp.find(u235)->second;
    return comp.find(u235)->second * fraction;
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct MatConverter2 : public Converter<Material> {
  MatConverter2() {}
  virtual ~MatConverter2() {}
  
  virtual double convert(Material::Ptr r) {
    const CompMap& comp = r->comp()->mass();
    double uamt = comp.find(u235)->second;
    return comp.find(u235)->second * fraction * fraction;
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, XlateCapacities) {
  Material::Ptr mat = get_mat(u235, qty);

  Converter<Material>::Ptr c1(new MatConverter1());
  double qty1 = 2.5 * qty;
  CapacityConstraint<Material> cc1(qty1, c1);

  Converter<Material>::Ptr c2(new MatConverter2());
  double qty2 = 0.8 * qty;
  CapacityConstraint<Material> cc2(qty2, c2);
  
  CapacityConstraint<Material> carr1[] = {cc1, cc2};
  std::set< CapacityConstraint<Material> >
      rconstrs(carr1, carr1 + sizeof(carr1) / sizeof(carr1[0]));

  CapacityConstraint<Material> carr2[] = {cc1};
  std::set< CapacityConstraint<Material> >
      bconstrs(carr2, carr2 + sizeof(carr2) / sizeof(carr2[0]));

  ExchangeNode::Ptr rnode(new ExchangeNode());
  ExchangeNode::Ptr bnode(new ExchangeNode());
  Arc arc(rnode, bnode);

  double rarr[] = {(c1->convert(mat) / qty1), (c2->convert(mat) / qty2)};
  std::vector<double> rexp(rarr, rarr +sizeof(rarr) / sizeof(rarr[0]));
      
  double barr[] = {(c1->convert(mat) / qty1)};
  std::vector<double> bexp(barr, barr +sizeof(barr) / sizeof(barr[0]));
      
  TranslateCapacities<Material>(mat, rconstrs, rnode, arc);
  EXPECT_EQ(rexp, rnode->unit_capacities[arc]);

  TranslateCapacities<Material>(mat, bconstrs, bnode, arc);
  EXPECT_EQ(bexp, bnode->unit_capacities[arc]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, XlateReq) {
  Converter<Material>::Ptr c1(new MatConverter1());
  double qty1 = 2.5 * qty;
  CapacityConstraint<Material> cc1(qty1, c1);

  Converter<Material>::Ptr c2(new MatConverter2());
  double qty2 = 0.8 * qty;
  CapacityConstraint<Material> cc2(qty2, c2);
    
  double carr[] = {qty1, qty2};
  std::vector<double> cexp(carr, carr + sizeof(carr) / sizeof(carr[0]));
  
  
  RequestPortfolio<Material>::Ptr rp(new RequestPortfolio<Material>());
  Request<Material>::Ptr req = rp->AddRequest(get_mat(u235, qty), &trader);
  rp->AddConstraint(cc1);
  rp->AddConstraint(cc2);

  ExchangeContext<Material> ctx;
  ExchangeTranslator<Material> xlator(&ctx);

  RequestGroup::Ptr set = TranslateRequestPortfolio(xlator.translation_ctx(), rp);

  EXPECT_EQ(qty, set->qty());
  EXPECT_EQ(cexp, set->capacities());
  EXPECT_TRUE(xlator.translation_ctx().request_to_node.find(req)
              != xlator.translation_ctx().request_to_node.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, XlateBid) {
  Request<Material>::Ptr req =
      Request<Material>::Create(get_mat(u235, qty), &trader);
  
  Converter<Material>::Ptr c1(new MatConverter1());
  double qty1 = 2.5 * qty;
  CapacityConstraint<Material> cc1(qty1, c1);

  Converter<Material>::Ptr c2(new MatConverter2());
  double qty2 = 0.8 * qty;
  CapacityConstraint<Material> cc2(qty2, c2);
  
  double carr[] = {qty1, qty2};
  std::vector<double> cexp(carr, carr + sizeof(carr) / sizeof(carr[0]));
  
  BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());
  Bid<Material>::Ptr bid = port->AddBid(req, get_mat(u235, qty), &trader);
  port->AddConstraint(cc1);
  port->AddConstraint(cc2);

  ExchangeContext<Material> ctx;
  ExchangeTranslator<Material> xlator(&ctx);

  ExchangeNodeGroup::Ptr set =
      TranslateBidPortfolio(xlator.translation_ctx(), port);

  EXPECT_EQ(cexp, set->capacities());
  EXPECT_TRUE(xlator.translation_ctx().bid_to_node.find(bid)
              != xlator.translation_ctx().bid_to_node.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, XlateArc) {
  Material::Ptr mat = get_mat(u235, qty);

  Converter<Material>::Ptr c1(new MatConverter1());
  double qty1 = 2.5 * qty;
  CapacityConstraint<Material> cc1(qty1, c1);

  Converter<Material>::Ptr c2(new MatConverter2());
  double qty2 = 0.8 * qty;
  CapacityConstraint<Material> cc2(qty2, c2);
  
  double carr[] = {qty1, qty2};
  std::vector<double> cexp(carr, carr + sizeof(carr) / sizeof(carr[0]));

  RequestPortfolio<Material>::Ptr rport(new RequestPortfolio<Material>());
  Request<Material>::Ptr req = rport->AddRequest(get_mat(u235, qty), &trader);
  rport->AddConstraint(cc1);
  
  BidPortfolio<Material>::Ptr bport(new BidPortfolio<Material>());
  Bid<Material>::Ptr bid = bport->AddBid(req, get_mat(u235, qty), &trader);
  bport->AddConstraint(cc1);
  bport->AddConstraint(cc2);
    
  ExchangeContext<Material> ctx;
  ExchangeTranslator<Material> xlator(&ctx);

  // give the xlator the correct state
  RequestGroup::Ptr rset =
      TranslateRequestPortfolio(xlator.translation_ctx(), rport);
  ExchangeNodeGroup::Ptr bset =
      TranslateBidPortfolio(xlator.translation_ctx(), bport);

  Arc a = TranslateArc(xlator.translation_ctx(), bid);

  EXPECT_EQ(xlator.translation_ctx().bid_to_node[bid], a.second);
  EXPECT_EQ(xlator.translation_ctx().request_to_node[req], a.first);

  double barr[] = {(c1->convert(mat) / qty1), (c2->convert(mat) / qty2)};
  std::vector<double> bexp(barr, barr +sizeof(barr) / sizeof(barr[0]));
  EXPECT_EQ(bexp, a.second->unit_capacities[a]);
      
  double rarr[] = {(c1->convert(mat) / qty1)};
  std::vector<double> rexp(rarr, rarr +sizeof(rarr) / sizeof(rarr[0]));
  EXPECT_EQ(rexp, a.first->unit_capacities[a]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, SimpleXlate) {
  std::string commod = "c";
  double pref = 4.5;
  RequestPortfolio<Material>::Ptr rport(new RequestPortfolio<Material>());
  Request<Material>::Ptr req =
      rport->AddRequest(get_mat(u235, qty), &trader, commod, pref);

  BidPortfolio<Material>::Ptr bport(new BidPortfolio<Material>());
  bport->AddBid(req, get_mat(u235, qty), &trader);

  ExchangeContext<Material> ctx;
  ctx.AddRequestPortfolio(rport);
  ctx.AddBidPortfolio(bport);
  
  ExchangeTranslator<Material> xlator(&ctx);
  
  ExchangeGraph::Ptr graph;
  EXPECT_NO_THROW(graph = xlator.Translate());
  EXPECT_EQ(1, graph->request_groups().size());
  EXPECT_EQ(1, graph->supply_groups().size());
  EXPECT_EQ(2, graph->node_arc_map().size());
  EXPECT_EQ(1, graph->arcs().size());
  EXPECT_EQ(0, graph->matches().size());
  const Arc& a = *graph->arcs().begin();
  EXPECT_EQ(pref, a.first->prefs[a]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExXlateTests, BackXlate) {
  ExchangeContext<Material> ctx;
  ExchangeTranslator<Material> xlator(&ctx);
  
  Request<Material>::Ptr ur(get_req());
  Request<Material>::Ptr xr(get_req());
  Bid<Material>::Ptr vb(get_bid());
  Bid<Material>::Ptr yb(get_bid());
  
  ExchangeNode::Ptr u(new ExchangeNode());  
  ExchangeNode::Ptr v(new ExchangeNode());
  ExchangeNode::Ptr x(new ExchangeNode());  
  ExchangeNode::Ptr y(new ExchangeNode());

  AddRequest(xlator.translation_ctx(), ur, u);
  AddRequest(xlator.translation_ctx(), xr, x);
  AddBid(xlator.translation_ctx(), vb, v);
  AddBid(xlator.translation_ctx(), yb, y);
  
  Arc a(u, v);
  Arc b(x, y);

  double qty = 2.5; // some magic numbers
  double aqty = qty * 0.1;
  double bqty = qty * 1.5;

  Trade<Material> aexp(ur, vb, aqty);
  Trade<Material> bexp(xr, yb, bqty);
  
  Trade<Material> tarr[] = {aexp, bexp};
  std::vector< Trade<Material> > exp(tarr, tarr + sizeof(tarr) / sizeof(tarr[0]));
  
  Match amatch(std::make_pair(a, aqty));
  Match bmatch(std::make_pair(b, bqty));
  
  Match marr[] = {amatch, bmatch};
  std::vector<Match> matches(marr, marr + sizeof(marr) / sizeof(marr[0]));

  std::vector< Trade<Material> > obs;
  xlator.BackTranslateSolution(matches, obs);
  EXPECT_EQ(exp, obs);
}
