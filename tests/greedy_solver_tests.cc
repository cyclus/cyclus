#include <gtest/gtest.h>

#include "exchange_graph.h"
#include "greedy_preconditioner.h"
#include "greedy_solver.h"
#include "error.h"

using cyclus::Arc;
using cyclus::AvgPrefComp;
using cyclus::ExchangeGraph;
using cyclus::ExchangeNode;
using cyclus::ExchangeNodeGroup;
using cyclus::RequestGroup;
using cyclus::GreedySolver;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(GreedySolverTests, ExchangeNodeCapThrow) {
  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  
  Arc a(m, n);
  EXPECT_THROW(Capacity(m, a), cyclus::StateError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(GreedySolverTests, ExchangeNodeNoCap) {
  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);

  ExchangeNodeGroup ugroup;
  ugroup.AddExchangeNode(m);

  EXPECT_DOUBLE_EQ(Capacity(m, a), std::numeric_limits<double>::max());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(GreedySolverTests, DefaultCap) {
  double min = 2;
  ExchangeNode::Ptr u(new ExchangeNode(min));
  ExchangeNode::Ptr v(new ExchangeNode(min + 1));
  Arc a(u, v);
  ExchangeNodeGroup s;
  s.AddExchangeNode(u);
  s.AddExchangeNode(v);

  bool max = true;
  EXPECT_DOUBLE_EQ(min, Capacity(u, a, max));
  EXPECT_DOUBLE_EQ(min + 1, Capacity(v, a, !max));
  EXPECT_DOUBLE_EQ(min, Capacity(v, a, !max, 1));
}

TEST(GreedySolverTests, ExchangeNodeCaps1) {
  double ncap = 1.0;
  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);
  n->unit_capacities[a].push_back(ncap);

  double scap = 1.5;
  ExchangeNodeGroup s;
  s.AddCapacity(scap);
  s.AddExchangeNode(n);

  EXPECT_DOUBLE_EQ(scap, Capacity(n, a));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(GreedySolverTests, ExchangeNodeCaps2) {
  double qty = 1.5;
  double minucap = 1.7;
  double mincap = 5;
  double maxucap = .01;
  double maxcap = 1;

  double acap[] = {10, mincap, 3, maxcap};
  std::vector<double> caps (acap, acap + sizeof(acap) / sizeof(acap[0]) );

  double aucap[] = {2.1, minucap, .07, maxucap};
  std::vector<double> ucaps (aucap, aucap + sizeof(aucap) / sizeof(aucap[0]) );

  std::vector<double> exp;
  for (int i = 0; i < caps.size(); i++) {
    exp.push_back(caps[i] - ucaps[i] * qty);
  }

  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode());
  Arc a(m, n);
  n->unit_capacities[a] = ucaps;

  ExchangeNodeGroup s;
  for (int i = 0; i < caps.size(); i++) {
    s.AddCapacity(caps[i]);
  }
  s.AddExchangeNode(n);
  bool min = true;
  EXPECT_EQ(mincap / minucap, Capacity(n, a, min));
  EXPECT_EQ(maxcap / maxucap, Capacity(n, a, !min));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(GreedySolverTests, ExchangeNodeCaps3) {
  double ncap = 1.0;
  double qty = 0.5;

  ExchangeNode::Ptr m(new ExchangeNode());
  ExchangeNode::Ptr n(new ExchangeNode(0.5));
  EXPECT_EQ(n->qty, 0.5);

  Arc a(m, n);
  n->unit_capacities[a].push_back(ncap);

  double scap = 1.5;
  ExchangeNodeGroup s;
  s.AddCapacity(scap);
  s.AddExchangeNode(n);

  EXPECT_DOUBLE_EQ(qty, Capacity(n, a));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(GreedySolverTests, ArcCap) {
  double uval = 1.0;
  double vval = 0.5;

  ExchangeNode::Ptr u(new ExchangeNode());

  ExchangeNode::Ptr v(new ExchangeNode());
  Arc a(u, v);

  u->unit_capacities[a].push_back(uval);
  v->unit_capacities[a].push_back(vval);

  ExchangeNodeGroup ugroup;
  ugroup.AddExchangeNode(u);
  double ucap = uval * 1.5;
  ugroup.AddCapacity(ucap);

  ExchangeNodeGroup vgroup;
  vgroup.AddExchangeNode(v);
  double vcap = vval;
  vgroup.AddCapacity(vcap);


  EXPECT_DOUBLE_EQ(Capacity(a), 1.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(GreedySolverTests, AvgPref) {
  ExchangeNode::Ptr u1(new ExchangeNode());
  ExchangeNode::Ptr u2(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());

  Arc a1(u1, v);
  Arc a2(u2, v);

  u1->prefs[a1] = 1;
  u2->prefs[a2] = 2;

  std::vector<ExchangeNode::Ptr> nodes;
  nodes.push_back(u1);
  nodes.push_back(u2);

  EXPECT_EQ(nodes[0], u1);
  EXPECT_EQ(nodes[1], u2);
  std::sort(nodes.begin(), nodes.end(), AvgPrefComp);
  EXPECT_EQ(nodes[0], u2);
  EXPECT_EQ(nodes[1], u1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(GreedySolverTests, Condition) {
  ExchangeNode::Ptr u1(new ExchangeNode());
  ExchangeNode::Ptr u2(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());

  Arc a1(u1, v);
  Arc a2(u2, v);

  u1->prefs[a1] = 1;
  u2->prefs[a2] = 2;

  RequestGroup::Ptr gu1(new RequestGroup());
  gu1->AddExchangeNode(u1);
  RequestGroup::Ptr gu2(new RequestGroup());
  gu2->AddExchangeNode(u2);
  ExchangeNodeGroup::Ptr gv(new ExchangeNodeGroup());
  gv->AddExchangeNode(v);

  ExchangeGraph g;
  g.AddRequestGroup(gu1);
  g.AddRequestGroup(gu2);
  g.AddSupplyGroup(gv);

  EXPECT_EQ(g.request_groups()[0], gu1);
  EXPECT_EQ(g.request_groups()[1], gu2);

  GreedySolver s;
  s.graph(&g);
  s.Condition();

  EXPECT_EQ(g.request_groups()[1], gu1);
  EXPECT_EQ(g.request_groups()[0], gu2);
}
