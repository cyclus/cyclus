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
using cyclus::GreedyPreconditioner;

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
TEST(GreedySolverTests, General) {
  ExchangeNode::Ptr u1(new ExchangeNode());
  ExchangeNode::Ptr u2(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());

  Arc a1(u1, v);
  Arc a2(u2, v);

  u1->prefs[a1] = 1;
  u1->unit_capacities[a1].push_back(1);
  u2->prefs[a2] = 2;
  u2->unit_capacities[a2].push_back(1);
  v->unit_capacities[a1].push_back(1);
  v->unit_capacities[a2].push_back(1);
  
  RequestGroup::Ptr gu1(new RequestGroup());
  gu1->AddExchangeNode(u1);
  gu1->AddCapacity(1);
  RequestGroup::Ptr gu2(new RequestGroup());
  gu2->AddExchangeNode(u2);
  gu2->AddCapacity(2);
  ExchangeNodeGroup::Ptr gv(new ExchangeNodeGroup());
  gv->AddExchangeNode(v);
  gv->AddCapacity(1.5);
  
  ExchangeGraph g;
  g.AddRequestGroup(gu1);
  g.AddRequestGroup(gu2);
  g.AddSupplyGroup(gv);

  EXPECT_EQ(g.request_groups()[0], gu1);
  EXPECT_EQ(g.request_groups()[1], gu2);

  std::map<std::string, double> null_weights;
  // the solver cleans up
  GreedyPreconditioner* p = new GreedyPreconditioner(null_weights);
  GreedySolver s(false, p);

  s.graph(&g);
  s.Init();
  EXPECT_EQ(s.Capacity(a1), 1);
  EXPECT_EQ(s.Capacity(a2), 1.5);
  
  s.Condition();  
  EXPECT_EQ(g.request_groups()[1], gu1);
  EXPECT_EQ(g.request_groups()[0], gu2);
}
