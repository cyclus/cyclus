#include <gtest/gtest.h>

#include "cyc_limits.h"
#include "error.h"

#include "exchange_graph.h"

using cyclus::Arc;
using cyclus::ExchangeGraph;
using cyclus::Match;
using cyclus::ExchangeNode;
using cyclus::ExchangeNodeSet;
using cyclus::RequestSet;
using std::vector;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeSets) {
  ExchangeNode::Ptr n(new ExchangeNode());
  ExchangeNodeSet s;
  s.AddExchangeNode(n);
  EXPECT_EQ(&s, n->set);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ReqSets) {
  double q = 1.5;
  RequestSet r;
  EXPECT_EQ(0, r.qty);
  r = RequestSet(q);
  EXPECT_EQ(q, r.qty);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeCapThrow) {
  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);
  EXPECT_THROW(Capacity(m, a), cyclus::StateError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeNoCap) {
  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);
  
  ExchangeNodeSet uset;
  uset.AddExchangeNode(m);
    
  EXPECT_DOUBLE_EQ(Capacity(m, a), std::numeric_limits<double>::max());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeCaps1) {
  double ncap = 1.0;
  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);
  n->unit_capacities[a].push_back(ncap);

  double scap = 1.5;
  ExchangeNodeSet s;
  s.capacities.push_back(scap);
  s.AddExchangeNode(n);

  EXPECT_EQ(scap, Capacity(n, a));  
  double qty = 1.0;
  UpdateCapacity(n, a, qty);
  EXPECT_EQ(scap - qty, Capacity(n, a));  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeCaps2) {
  double qty = 1.5;
  double ucap = 1.7;
  double cap = 5;
  
  double acap[] = {10, cap, 3, 1};
  vector<double> caps (acap, acap + sizeof(acap) / sizeof(acap[0]) );
  
  double aucap[] = {2.1, ucap, .07, .01};
  vector<double> ucaps (aucap, aucap + sizeof(aucap) / sizeof(aucap[0]) );
  
  vector<double> exp;
  for (int i = 0; i < caps.size(); i++) {
    exp.push_back(caps[i] - ucaps[i] * qty);
  }

  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);
  n->unit_capacities[a] = ucaps;

  ExchangeNodeSet s;
  s.capacities = caps;
  s.AddExchangeNode(n);
  double min_exp = cap / ucap;
  EXPECT_EQ(min_exp, Capacity(n, a));

  UpdateCapacity(n, a, qty);
  EXPECT_EQ(exp, s.capacities);
  min_exp = (cap - qty * ucap) / ucap;
  EXPECT_EQ(min_exp, Capacity(n, a));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeCaps3) {
  double ncap = 1.0;
  double qty = 0.5;

  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode(0.5));
  EXPECT_EQ(n->max_qty, 0.5);
  
  Arc a(m, n);
  n->unit_capacities[a].push_back(ncap);

  double scap = 1.5;
  ExchangeNodeSet s;
  s.capacities.push_back(scap);
  s.AddExchangeNode(n);

  EXPECT_EQ(qty, Capacity(n, a));  
  UpdateCapacity(n, a, qty);
  EXPECT_EQ(n->qty, 0.5);
  EXPECT_EQ(0, Capacity(n, a));  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeUpdateThrow1) {
  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);

  double qty = 5;
  EXPECT_THROW(UpdateCapacity(n, a, qty), cyclus::StateError);  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeUpdateThrow2) {
  double qty = 10;
  double unit = 2;
  double min_diff = cyclus::eps() * (1 + cyclus::eps());
  double cap = qty * unit - min_diff;
  ASSERT_TRUE(cyclus::IsNegative(cap - qty * unit));
  
  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);
  n->unit_capacities[a].push_back(unit);

  ExchangeNodeSet s;
  s.capacities.push_back(cap);
  s.AddExchangeNode(n);
  
  EXPECT_THROW(UpdateCapacity(n, a, qty), cyclus::ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ExchangeNodeUpdateThrow3) {
  double qty = 5;
  
  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode(qty - 1));
  Arc a(m, n);

  ExchangeNodeSet s;
  s.AddExchangeNode(n);

  EXPECT_THROW(UpdateCapacity(n, a, qty), cyclus::ValueError);  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ArcCap) {
  double uval = 1.0;
  double vval = 0.5;
  
  ExchangeNode::Ptr u(new ExchangeNode());
  
  ExchangeNode::Ptr v(new ExchangeNode());
  Arc a(u, v);
  
  u->unit_capacities[a].push_back(uval);
  v->unit_capacities[a].push_back(vval);
  
  ExchangeNodeSet uset;
  uset.AddExchangeNode(u);
  double ucap = uval * 1.5;
  uset.capacities.push_back(ucap);
  
  ExchangeNodeSet vset;
  vset.AddExchangeNode(v);
  double vcap = vval;
  vset.capacities.push_back(vcap);
  
  
  EXPECT_DOUBLE_EQ(Capacity(a), 1.0);

  UpdateCapacity(u, a, uval);
  EXPECT_DOUBLE_EQ(Capacity(a), 0.5);  

  UpdateCapacity(v, a, 1.0);
  EXPECT_DOUBLE_EQ(Capacity(a), 0.0);  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddReqSet) {
  RequestSet::Ptr prs(new RequestSet());
  ExchangeGraph g;
  g.AddRequestSet(prs);
  EXPECT_EQ(g.request_sets[0], prs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddSuppSet) {
  ExchangeNodeSet::Ptr pss(new ExchangeNodeSet());
  ExchangeGraph g;
  g.AddSupplySet(pss);
  EXPECT_EQ(g.supply_sets[0], pss);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddArc1) {
  ExchangeGraph g;
  
  ExchangeNode::Ptr u(new ExchangeNode());  
  ExchangeNode::Ptr v(new ExchangeNode());

  Arc a(u, v);

  Arc arr[] = {a};
  vector<Arc> exp (arr, arr + sizeof(arr) / sizeof(arr[0]) );

  g.AddArc(a);
  EXPECT_EQ(exp, g.node_arc_map[u]);
  EXPECT_EQ(exp, g.node_arc_map[v]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddArc2) {
  ExchangeGraph g;
  
  ExchangeNode::Ptr u(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());
  ExchangeNode::Ptr w(new ExchangeNode());
  ExchangeNode::Ptr x(new ExchangeNode());

  Arc a1(u, v);
  Arc a2(u, w);
  Arc a3(x, w);

  Arc arru[] = {a1, a2};
  vector<Arc> expu (arru, arru + sizeof(arru) / sizeof(arru[0]) );
  
  Arc arrv[] = {a1};
  vector<Arc> expv (arrv, arrv + sizeof(arrv) / sizeof(arrv[0]) );
  
  Arc arrw[] = {a2, a3};
  vector<Arc> expw (arrw, arrw + sizeof(arrw) / sizeof(arrw[0]) );
  
  Arc arrx[] = {a3};
  vector<Arc> expx (arrx, arrx + sizeof(arrx) / sizeof(arrx[0]) );

  g.AddArc(a1);
  g.AddArc(a2);
  g.AddArc(a3);
  
  EXPECT_EQ(expu, g.node_arc_map[u]);
  EXPECT_EQ(expv, g.node_arc_map[v]);
  EXPECT_EQ(expw, g.node_arc_map[w]);
  EXPECT_EQ(expx, g.node_arc_map[x]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddMatch) {
  ExchangeGraph g;

  double uval = 1.0;
  double vval = 0.5;
  ExchangeNode::Ptr u(new ExchangeNode());  
  ExchangeNode::Ptr v(new ExchangeNode());
  Arc a(u, v);
  
  u->unit_capacities[a].push_back(uval);
  v->unit_capacities[a].push_back(vval);
  
  double large = 500;
  
  ExchangeNodeSet::Ptr uset(new ExchangeNodeSet());
  uset->AddExchangeNode(u);
  double ucap = uval * 500;
  uset->capacities.push_back(ucap);
  
  ExchangeNodeSet::Ptr vset(new ExchangeNodeSet());
  vset->AddExchangeNode(v);
  double vcap = vval * 500;
  vset->capacities.push_back(vcap);
  
  double qty = large * 0.1;
  
  Match match(std::make_pair(a, qty));
  
  Match arr[] = {match};
  std::vector<Match> exp (arr, arr + sizeof(arr) / sizeof(arr[0]));

  g.AddMatch(a, qty);
  EXPECT_EQ(exp, g.matches);
}
