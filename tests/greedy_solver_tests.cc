#include <gtest/gtest.h>

#include "exchange_graph.h"
#include "greedy_preconditioner.h"
#include "greedy_solver.h"
#include "error.h"

using cyclus::Arc;
using cyclus::AvgArcCostComp;
using cyclus::ExchangeGraph;
using cyclus::ExchangeNode;
using cyclus::ExchangeNodeGroup;
using cyclus::RequestGroup;
using cyclus::GreedySolver;
using cyclus::GreedyPreconditioner;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(GreedySolverTests, AvgCost) {
  ExchangeNode::Ptr u1(new ExchangeNode());
  ExchangeNode::Ptr u2(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());

  Arc a1(u1, v);
  Arc a2(u2, v);

  a1.arc_cost(1.0);
  a2.arc_cost(2.0); 

  ExchangeGraph g;
  RequestGroup::Ptr rg(new RequestGroup());
  rg->AddExchangeNode(u1);
  rg->AddExchangeNode(u2);
  g.AddRequestGroup(rg);
  g.AddArc(a1);
  g.AddArc(a2);

  std::vector<ExchangeNode::Ptr> nodes;
  nodes.push_back(u2);  
  nodes.push_back(u1);  

  EXPECT_EQ(nodes[0], u2);
  EXPECT_EQ(nodes[1], u1);
  std::sort(nodes.begin(), nodes.end(), AvgArcCostComp(&g));
  EXPECT_EQ(nodes[0], u1);
  EXPECT_EQ(nodes[1], u2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(GreedySolverTests, General) {
  ExchangeNode::Ptr u1(new ExchangeNode());
  ExchangeNode::Ptr u2(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());

  Arc a1(u1, v);
  Arc a2(u2, v);

  a1.arc_cost(1.0);
  a2.arc_cost(2.0);
  
  u1->unit_capacities[a1].push_back(1);
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
  g.AddArc(a1);
  g.AddArc(a2);

  EXPECT_EQ(g.request_groups()[0], gu1);
  EXPECT_EQ(g.request_groups()[1], gu2);

  bool excl = false;
  GreedySolver s(excl);

  s.graph(&g);
  s.Init();
  EXPECT_EQ(s.Capacity(a1), 1);
  EXPECT_EQ(s.Capacity(a2), 1.5);
  
  s.Condition();  
  // With ascending sort (lower weights first):
  // gu1 has lower weight (arc_cost 1.0 -> weight ~1.5) than gu2 (arc_cost 2.0 -> weight ~1.667)
  EXPECT_EQ(g.request_groups()[0], gu1);
  EXPECT_EQ(g.request_groups()[1], gu2);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(GreedySolverTests, PrioritizesCheapestNegativeCostRequest) {
  ExchangeNode::Ptr expensive_request(new ExchangeNode(1.0));
  ExchangeNode::Ptr cheap_request(new ExchangeNode(1.0));
  ExchangeNode::Ptr supplier(new ExchangeNode(1.0));

  Arc expensive_arc(expensive_request, supplier);
  expensive_arc.arc_cost(-0.5);
  Arc cheap_arc(cheap_request, supplier);
  cheap_arc.arc_cost(-2.0);

  expensive_request->unit_capacities[expensive_arc].push_back(1.0);
  cheap_request->unit_capacities[cheap_arc].push_back(1.0);
  supplier->unit_capacities[expensive_arc].push_back(1.0);
  supplier->unit_capacities[cheap_arc].push_back(1.0);

  RequestGroup::Ptr expensive_group(new RequestGroup(1.0));
  expensive_group->AddExchangeNode(expensive_request);
  expensive_group->AddCapacity(1.0);
  RequestGroup::Ptr cheap_group(new RequestGroup(1.0));
  cheap_group->AddExchangeNode(cheap_request);
  cheap_group->AddCapacity(1.0);
  ExchangeNodeGroup::Ptr supply_group(new ExchangeNodeGroup());
  supply_group->AddExchangeNode(supplier);
  supply_group->AddCapacity(1.0);

  ExchangeGraph g;
  g.AddRequestGroup(expensive_group);
  g.AddRequestGroup(cheap_group);
  g.AddSupplyGroup(supply_group);
  g.AddArc(expensive_arc);
  g.AddArc(cheap_arc);

  GreedySolver solver(false);
  solver.Solve(&g);

  ASSERT_EQ(1, g.matches().size());
  EXPECT_EQ(cheap_request, g.matches()[0].first.unode());
  EXPECT_DOUBLE_EQ(-2.0, g.matches()[0].first.arc_cost());
  EXPECT_DOUBLE_EQ(1.0, g.matches()[0].second);
}
