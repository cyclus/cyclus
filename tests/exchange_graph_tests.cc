#include <gtest/gtest.h>

#include "cyc_limits.h"
#include "error.h"

#include "exchange_graph.h"

using std::vector;
using cyclus::Arc;
using cyclus::Node;
using cyclus::NodeSet;
using cyclus::RequestSet;
using cyclus::ExchangeGraph;
using cyclus::Match;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, NodeSets) {
  Node::Ptr n = Node::Ptr(new Node());
  NodeSet s;
  s.AddNode(n);
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
TEST(ExGraphTests, NodeCapThrow) {
  Node::Ptr u = Node::Ptr(new Node());
  EXPECT_THROW(Capacity(u), cyclus::StateError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, NodeNoCap) {
  Node::Ptr u = Node::Ptr(new Node());
  NodeSet uset;
  uset.AddNode(u);
    
  EXPECT_DOUBLE_EQ(Capacity(u), std::numeric_limits<double>::max());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, NodeCaps1) {
  double ncap = 1.0;
  Node::Ptr n = Node::Ptr(new Node());
  n->unit_capacities.push_back(ncap);

  double scap = 1.5;
  NodeSet s;
  s.capacities.push_back(scap);
  s.AddNode(n);

  EXPECT_EQ(scap, Capacity(n));  
  double qty = 1.0;
  UpdateCapacity(n, qty);
  EXPECT_EQ(scap - qty, Capacity(n));  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, NodeCaps2) {
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

  Node::Ptr n = Node::Ptr(new Node());
  n->unit_capacities = ucaps;

  NodeSet s;
  s.capacities = caps;
  s.AddNode(n);
  double min_exp = cap / ucap;
  EXPECT_EQ(min_exp, Capacity(n));

  UpdateCapacity(n, qty);
  EXPECT_EQ(exp, s.capacities);
  min_exp = (cap - qty * ucap) / ucap;
  EXPECT_EQ(min_exp, Capacity(n));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, NodeUpdateThrow1) {
  Node::Ptr n = Node::Ptr(new Node());
  double qty = 5;
  EXPECT_THROW(UpdateCapacity(n, qty), cyclus::StateError);  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, NodeUpdateThrow2) {
  double qty = 10;
  double unit = 2;
  double min_diff = cyclus::eps() * (1 + cyclus::eps());
  double cap = qty * unit - min_diff;
  ASSERT_TRUE(cyclus::DoubleNeg(cap - qty * unit));
  
  Node::Ptr n = Node::Ptr(new Node());
  n->unit_capacities.push_back(unit);

  NodeSet s;
  s.capacities.push_back(cap);
  s.AddNode(n);
  
  EXPECT_THROW(UpdateCapacity(n, qty), cyclus::ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ArcCap) {
  double uval = 1.0;
  double vval = 0.5;
  
  Node::Ptr u = Node::Ptr(new Node());
  u->unit_capacities.push_back(uval);
  
  Node::Ptr v = Node::Ptr(new Node());
  v->unit_capacities.push_back(vval);

  NodeSet uset;
  uset.AddNode(u);
  double ucap = uval * 1.5;
  uset.capacities.push_back(ucap);
  
  NodeSet vset;
  vset.AddNode(v);
  double vcap = vval;
  vset.capacities.push_back(vcap);
  
  Arc a(u, v);
  
  EXPECT_DOUBLE_EQ(Capacity(a), 1.0);

  UpdateCapacity(u, uval);
  EXPECT_DOUBLE_EQ(Capacity(a), 0.5);  

  UpdateCapacity(v, 1.0);
  EXPECT_DOUBLE_EQ(Capacity(a), 0.0);  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddReqSet) {
  RequestSet::Ptr prs = RequestSet::Ptr(new RequestSet());
  ExchangeGraph g;
  g.AddRequestSet(prs);
  EXPECT_EQ(g.request_sets[0], prs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddSuppSet) {
  NodeSet::Ptr pss = NodeSet::Ptr(new NodeSet());
  ExchangeGraph g;
  g.AddSupplySet(pss);
  EXPECT_EQ(g.supply_sets[0], pss);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddArc1) {
  ExchangeGraph g;
  
  Node::Ptr u = Node::Ptr(new Node());  
  Node::Ptr v = Node::Ptr(new Node());

  Arc::Ptr a = Arc::Ptr(new Arc(u, v));

  Arc::Ptr arr[] = {a};
  vector<Arc::Ptr> exp (arr, arr + sizeof(arr) / sizeof(arr[0]) );

  g.AddArc(a);
  EXPECT_EQ(exp, g.node_arc_map[u]);
  EXPECT_EQ(exp, g.node_arc_map[v]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, AddArc2) {
  ExchangeGraph g;
  
  Node::Ptr u = Node::Ptr(new Node());
  Node::Ptr v = Node::Ptr(new Node());
  Node::Ptr w = Node::Ptr(new Node());
  Node::Ptr x = Node::Ptr(new Node());

  Arc::Ptr a1 = Arc::Ptr(new Arc(u, v));
  Arc::Ptr a2 = Arc::Ptr(new Arc(u, w));
  Arc::Ptr a3 = Arc::Ptr(new Arc(x, w));

  Arc::Ptr arru[] = {a1, a2};
  vector<Arc::Ptr> expu (arru, arru + sizeof(arru) / sizeof(arru[0]) );
  
  Arc::Ptr arrv[] = {a1};
  vector<Arc::Ptr> expv (arrv, arrv + sizeof(arrv) / sizeof(arrv[0]) );
  
  Arc::Ptr arrw[] = {a2, a3};
  vector<Arc::Ptr> expw (arrw, arrw + sizeof(arrw) / sizeof(arrw[0]) );
  
  Arc::Ptr arrx[] = {a3};
  vector<Arc::Ptr> expx (arrx, arrx + sizeof(arrx) / sizeof(arrx[0]) );

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
  Node::Ptr u = Node::Ptr(new Node());
  u->unit_capacities.push_back(uval);
  
  double vval = 0.5;
  Node::Ptr v = Node::Ptr(new Node());
  v->unit_capacities.push_back(vval);

  double large = 500;
  
  NodeSet::Ptr uset = NodeSet::Ptr(new NodeSet());
  uset->AddNode(u);
  double ucap = uval * 500;
  uset->capacities.push_back(ucap);
  
  NodeSet::Ptr vset = NodeSet::Ptr(new NodeSet());
  vset->AddNode(v);
  double vcap = vval * 500;
  vset->capacities.push_back(vcap);
  
  Arc::Ptr a = Arc::Ptr(new Arc(u, v));

  double qty = large * 0.1;
  
  Match match(std::make_pair(a, qty));
  
  Match arr[] = {match};
  std::vector<Match> exp (arr, arr + sizeof(arr) / sizeof(arr[0]));

  g.AddMatch(a, qty);
  EXPECT_EQ(exp, g.matches);
}
