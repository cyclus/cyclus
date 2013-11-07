#include <gtest/gtest.h>

#include "cyc_limits.h"
#include "error.h"

#include "exchange_graph.h"

using std::vector;
using cyclus::Arc;
using cyclus::Node;
using cyclus::NodeSet;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, NodeSets) {
  Node::Ptr n = Node::Ptr(new Node());
  NodeSet s;
  s.AddNode(n);
  EXPECT_EQ(&s, n->set);
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

  EXPECT_EQ(scap, s.capacities[0]);  
  double qty = 1.0;
  s.UpdateCapacities(n, qty);
  EXPECT_EQ(scap - qty, s.capacities[0]);  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, NodeCaps2) {
  double qty = 1.5;
  
  double acap[] = {10, 5, 3, 1};
  vector<double> caps (acap, acap + sizeof(acap) / sizeof(acap[0]) );
  
  double aucap[] = {2.1, 1.5, .07, .01};
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

  s.UpdateCapacities(n, qty);
  EXPECT_EQ(exp, s.capacities);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, NodeCapThrow) {
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ArcSetThrow) {
  Node::Ptr u = Node::Ptr(new Node());
  Node::Ptr v = Node::Ptr(new Node());

  Arc a;
  a.unode = u;
  a.vnode = v;
  
  EXPECT_THROW(a.capacity(), cyclus::StateError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, ArcNoCap) {
  Node::Ptr u = Node::Ptr(new Node());
  Node::Ptr v = Node::Ptr(new Node());

  NodeSet uset;
  uset.AddNode(u);
  NodeSet vset;
  vset.AddNode(v);
  
  Arc a;
  a.unode = u;
  a.vnode = v;
  
  EXPECT_DOUBLE_EQ(a.capacity(), std::numeric_limits<double>::max());
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
  
  EXPECT_DOUBLE_EQ(a.capacity(), 1.0);

  uset.UpdateCapacities(u, uval);
  EXPECT_DOUBLE_EQ(a.capacity(), 0.5);  

  vset.UpdateCapacities(v, 1.0);
  EXPECT_DOUBLE_EQ(a.capacity(), 0.0);  
}
