#include <gtest/gtest.h>

#include <cmath>

#include "exchange_graph.h"
#include "greedy_preconditioner.h"

using cyclus::Arc;
using cyclus::AvgCost;
using cyclus::ExchangeNode;
using cyclus::ExchangeNodeGroup;
using cyclus::ExchangeGraph;
using cyclus::GreedyPreconditioner;
using cyclus::NodeWeight;
using cyclus::RequestGroup;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ConditionerTests, AvgCost) {
  ExchangeNode::Ptr u1(new ExchangeNode());
  ExchangeNode::Ptr u2(new ExchangeNode());
  ExchangeNode::Ptr u3(new ExchangeNode());
  ExchangeNode::Ptr v1(new ExchangeNode());
  ExchangeNode::Ptr v2(new ExchangeNode());

  // u1 has two arcs with average arc cost of (1.0 + 3.0) / 2 = 2.0
  Arc a1(u1, v1);
  a1.arc_cost(1.0);
  Arc a2(u1, v2);
  a2.arc_cost(3.0);

  // u2 has one arc with arc cost 1.5
  Arc a3(u2, v1);
  a3.arc_cost(1.5);

  ExchangeGraph g;
  RequestGroup::Ptr rg(new RequestGroup());
  rg->AddExchangeNode(u1);
  rg->AddExchangeNode(u2);
  rg->AddExchangeNode(u3);
  g.AddRequestGroup(rg);
  ExchangeNodeGroup::Ptr sg(new ExchangeNodeGroup());
  sg->AddExchangeNode(v1);
  sg->AddExchangeNode(v2);
  g.AddSupplyGroup(sg);
  g.AddArc(a1);
  g.AddArc(a2);
  g.AddArc(a3);

  // Test average calculation: u1 should have average of (1.0 + 3.0) / 2 = 2.0
  EXPECT_DOUBLE_EQ(AvgCost(u1, &g), 2.0);

  // Test single arc: u2 should have average of 1.5
  EXPECT_DOUBLE_EQ(AvgCost(u2, &g), 1.5);

  // Test node with no arcs returns 0.0
  EXPECT_DOUBLE_EQ(AvgCost(u3, &g), 0.0);
  EXPECT_EQ(0u, g.node_arc_map().count(u3));

  // Test ordering: u1 (2.0) > u2 (1.5) > u3 (0.0)
  EXPECT_TRUE(AvgCost(u1, &g) > AvgCost(u2, &g));
  EXPECT_TRUE(AvgCost(u2, &g) > AvgCost(u3, &g));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ConditionerTests, NegativeCostsAreFiniteAndMonotonic) {
  ExchangeNode::Ptr zero_cost(new ExchangeNode());
  ExchangeNode::Ptr negative_one_cost(new ExchangeNode());
  ExchangeNode::Ptr negative_two_cost(new ExchangeNode());
  ExchangeNode::Ptr supplier(new ExchangeNode());

  Arc zero_arc(zero_cost, supplier);
  zero_arc.arc_cost(0.0);
  Arc negative_one_arc(negative_one_cost, supplier);
  negative_one_arc.arc_cost(-1.0);
  Arc negative_two_arc(negative_two_cost, supplier);
  negative_two_arc.arc_cost(-2.0);

  RequestGroup::Ptr requests(new RequestGroup());
  requests->AddExchangeNode(zero_cost);
  requests->AddExchangeNode(negative_one_cost);
  requests->AddExchangeNode(negative_two_cost);
  ExchangeNodeGroup::Ptr supplies(new ExchangeNodeGroup());
  supplies->AddExchangeNode(supplier);

  ExchangeGraph g;
  g.AddRequestGroup(requests);
  g.AddSupplyGroup(supplies);
  g.AddArc(zero_arc);
  g.AddArc(negative_one_arc);
  g.AddArc(negative_two_arc);

  std::map<std::string, double> weights;
  double zero_weight = NodeWeight(zero_cost, &weights, AvgCost(zero_cost, &g));
  double negative_one_weight =
      NodeWeight(negative_one_cost, &weights, AvgCost(negative_one_cost, &g));
  double negative_two_weight =
      NodeWeight(negative_two_cost, &weights, AvgCost(negative_two_cost, &g));

  EXPECT_DOUBLE_EQ(0.0, AvgCost(zero_cost, &g));
  EXPECT_DOUBLE_EQ(-1.0, AvgCost(negative_one_cost, &g));
  EXPECT_DOUBLE_EQ(-2.0, AvgCost(negative_two_cost, &g));
  EXPECT_TRUE(std::isfinite(zero_weight));
  EXPECT_TRUE(std::isfinite(negative_one_weight));
  EXPECT_TRUE(std::isfinite(negative_two_weight));
  EXPECT_LT(negative_two_weight, negative_one_weight);
  EXPECT_LT(negative_one_weight, zero_weight);

  GreedyPreconditioner conditioner;
  conditioner.Condition(&g);
  EXPECT_EQ(negative_two_cost, requests->nodes().at(0));
  EXPECT_EQ(negative_one_cost, requests->nodes().at(1));
  EXPECT_EQ(zero_cost, requests->nodes().at(2));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ConditionerTests, Conditioning) {
  ExchangeGraph g;

  ExchangeNode::Ptr n11(new ExchangeNode());
  n11->commod = "eggs";
  ExchangeNode::Ptr n12(new ExchangeNode());
  n12->commod = "spam";
  ExchangeNode::Ptr n13(new ExchangeNode());
  n13->commod = "eggs";
  double n1ecost = 1.0/4.0;
  double n1scost = 3.0/4.0;

  RequestGroup::Ptr g1(new RequestGroup());
  g1->AddExchangeNode(n11);
  g1->AddExchangeNode(n12);
  g1->AddExchangeNode(n13);
  g.AddRequestGroup(g1);

  ExchangeNode::Ptr n21(new ExchangeNode());
  n21->commod = "eggs";
  ExchangeNode::Ptr n22(new ExchangeNode());
  n22->commod = "spam";
  double n2ecost = 1;
  double n2scost = 1;

  RequestGroup::Ptr g2(new RequestGroup());
  g2->AddExchangeNode(n21);
  g2->AddExchangeNode(n22);
  g.AddRequestGroup(g2);

  ExchangeNodeGroup::Ptr s(new ExchangeNodeGroup);
  ExchangeNode::Ptr eggs(new ExchangeNode());
  ExchangeNode::Ptr spam(new ExchangeNode());
  s->AddExchangeNode(eggs);
  s->AddExchangeNode(spam);
  g.AddSupplyGroup(s);

  Arc n11e(n11, eggs);
  Arc n12s(n12, spam);
  Arc n13s(n13, spam);
  Arc n21e(n21, eggs);
  Arc n22s(n22, spam);

  // Set arc costs - this is what AvgCost reads
  n11e.arc_cost(n1ecost);
  n12s.arc_cost(n1scost);
  n13s.arc_cost(n1scost);
  n21e.arc_cost(n2ecost);
  n22s.arc_cost(n2scost);

  g.AddArc(n11e);
  g.AddArc(n12s);
  g.AddArc(n13s);
  g.AddArc(n21e);
  g.AddArc(n22s);

  // initial state
  EXPECT_EQ(g.request_groups().at(0), g1);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(0), n11);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(1), n12);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(2), n13);
  EXPECT_EQ(g.request_groups().at(1), g2);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(0), n21);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(1), n22);

  std::map<std::string, double> weights;
  weights["spam"] = 5.;
  weights["eggs"] = 2.;
  GreedyPreconditioner gp(weights);

  std::map<ExchangeNode::Ptr, double> avg_costs;

  double avg1 = 5./12;
  double avg2 = 2./2;
  double c1e = (1. + n1ecost / (1 + n1ecost));
  double c1s = (1. + n1scost / (1 + n1scost));
  double c2e = (1. + n2ecost / (1 + n2ecost));
  double c2s = (1. + n2scost / (1 + n2scost));
  avg_costs[n11] = AvgCost(n11, &g);
  avg_costs[n12] = AvgCost(n12, &g);
  avg_costs[n13] = AvgCost(n13, &g);
  avg_costs[n21] = AvgCost(n21, &g);
  avg_costs[n22] = AvgCost(n22, &g);

  double exp11 = c1e * weights[n11->commod];
  double exp12 = c1s * weights[n12->commod];
  double exp13 = c1s * weights[n13->commod];
  double exp21 = c2e * weights[n21->commod];
  double exp22 = c2s * weights[n22->commod];
  EXPECT_DOUBLE_EQ(NodeWeight(n11, &weights, avg_costs[n11]), exp11);
  EXPECT_DOUBLE_EQ(NodeWeight(n12, &weights, avg_costs[n12]), exp12);
  EXPECT_DOUBLE_EQ(NodeWeight(n13, &weights, avg_costs[n13]), exp13);
  EXPECT_DOUBLE_EQ(NodeWeight(n21, &weights, avg_costs[n21]), exp21);
  EXPECT_DOUBLE_EQ(NodeWeight(n22, &weights, avg_costs[n22]), exp22);

  double expg1 = (exp11 + exp12 + exp13) / 3;
  double expg2 = (exp21 + exp22) / 2;
  EXPECT_DOUBLE_EQ(GroupWeight(g1, &weights, &avg_costs), expg1);
  EXPECT_DOUBLE_EQ(GroupWeight(g2, &weights, &avg_costs), expg2);

  gp.Condition(&g);

  // final state
  EXPECT_EQ(g.request_groups().at(0), g1);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(0), n11);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(1), n13);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(2), n12);
  EXPECT_EQ(g.request_groups().at(1), g2);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(0), n21);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(1), n22);
}
