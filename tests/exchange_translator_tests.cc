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

using cyclus::Resource;
using cyclus::Material;
using cyclus::Request;
using cyclus::Bid;
using cyclus::Match;
using cyclus::Arc;
using cyclus::Node;
using cyclus::Trade;
using cyclus::ExchangeContext;
using cyclus::ExchangeTranslator;
using cyclus::CompMap;
using cyclus::Composition;
using cyclus::CapacityConstraint;

// ----- xlate helpers ------
double fraction = 0.7;
int u235 = 92235;

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

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TEST(ExXlateTests, XlateBid) {
//   cyclus::CompMap cm;
//   double qty = 6.3;
//   cm[92235] = qty;
//   Composition::Ptr comp = Composition::CreateFromMass(cm);
//   Material::Ptr mat = Material::CreateUntracked(qty, comp);

//   CapacityConstraint<Material> cc1;
//   cc1.capacity = qty;
//   cc1.converter = &converter1;

//   CapacityConstraint<Material> cc2;
//   cc2.capacity = qty;
//   cc2.converter = &converter2;
  
//   CapacityConstraint<Material> carr[] = {cc1, cc2};
//   std::set< CapacityConstraint<Material> >
//       constraints(carr, carr + sizeof(carr) / sizeof(carr[0]));

//   Bid<Material>::Ptr bid(new Bid<Material>);
//   bid->offer = mat;
  
//   Node::Ptr exp(new Node());
//   exp->unit_capacities.push_back(converter1(mat) / qty);
//   exp->unit_capacities.push_back(converter2(mat) / qty);

//   ExchangeContext<Material> ctx;
//   ExchangeTranslator<Material> xlator(&ctx);
//   Node::Ptr obs = xlator.__TranslateBid(bid, constraints);
//   EXPECT_EQ(*exp.get(), *obs.get());
// }

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
