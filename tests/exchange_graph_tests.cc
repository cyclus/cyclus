#include <gtest/gtest.h>

#include "cyc_limits.h"
#include "error.h"

#include "exchange_graph.h"

using std::vector;
using cyclus::Arc;
using cyclus::Node;
using cyclus::NodeSet;
using cyclus::RequestSet;

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
  s.UpdateCapacities(n, qty);
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

  s.UpdateCapacities(n, qty);
  EXPECT_EQ(exp, s.capacities);
  min_exp = (cap - qty * ucap) / ucap;
  EXPECT_EQ(min_exp, Capacity(n));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, NodeUpdateThrow) {
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
  
  EXPECT_THROW(s.UpdateCapacities(n, qty), cyclus::ValueError);
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
  
  Arc a;
  a.unode = u;
  a.vnode = v;
  
  EXPECT_DOUBLE_EQ(Capacity(a), 1.0);

  uset.UpdateCapacities(u, uval);
  EXPECT_DOUBLE_EQ(Capacity(a), 0.5);  

  vset.UpdateCapacities(v, 1.0);
  EXPECT_DOUBLE_EQ(Capacity(a), 0.0);  
}
