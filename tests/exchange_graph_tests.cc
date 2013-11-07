#include <gtest/gtest.h>

#include "cyc_limits.h"
#include "error.h"

#include "exchange_graph.h"

using std::vector;
using cyclus::Node;
using cyclus::NodeSet;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExGraphTests, NodeCaps) {
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
TEST(ExGraphTests, NodeCapAssert) {
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
