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
   // This test checks that a set of exchange nodes in a graph can be correctly
  // assigned commod_weights and sorted by the greedy preconditioner, and that 
  // the resulting order of nodes and groups can be different than the initial
  // ordering (the sort succeeded), and that they are in ascending order by
  // average cost. 

  ExchangeGraph g;

  // Some of the request nodes will request eggs, some will request spam. There
  // is no requirement that the "egg" nodes recieve eggs, etc, but this allows 
  // us to check different weights.
  ExchangeNode::Ptr n1(new ExchangeNode());
  n1->commod = "eggs";
  ExchangeNode::Ptr n2(new ExchangeNode());
  n2->commod = "eggs";
  ExchangeNode::Ptr n3(new ExchangeNode());
  n3->commod = "eggs";
  ExchangeNode::Ptr n4(new ExchangeNode());
  n4->commod = "eggs";
  ExchangeNode::Ptr n5(new ExchangeNode());
  n5->commod = "eggs";

  ExchangeNode::Ptr n6(new ExchangeNode());
  n6->commod = "spam";
  ExchangeNode::Ptr n7(new ExchangeNode());
  n7->commod = "spam";
  ExchangeNode::Ptr n8(new ExchangeNode());
  n8->commod = "spam";
  ExchangeNode::Ptr n9(new ExchangeNode());
  n9->commod = "spam";

  // We make three groups and add the nodes to each group. I tried to make sure
  // each group got some spam and some eggs.
  RequestGroup::Ptr g1(new RequestGroup());
  g1->AddExchangeNode(n1);
  g1->AddExchangeNode(n3);
  g1->AddExchangeNode(n5);
  g.AddRequestGroup(g1);

  RequestGroup::Ptr g2(new RequestGroup());
  g2->AddExchangeNode(n2);
  g2->AddExchangeNode(n7);
  g2->AddExchangeNode(n8);
  g2->AddExchangeNode(n9);
  g.AddRequestGroup(g2);

  RequestGroup::Ptr g3(new RequestGroup());
  g3->AddExchangeNode(n4);
  g3->AddExchangeNode(n6);
  g.AddRequestGroup(g3);

  // Create cost modifiers for each of the consumers to differentiate them
  double n1_cost_mod = 0.1;
  double n2_cost_mod = 0.15;
  double n3_cost_mod = 0.2;
  double n4_cost_mod = 0.25;
  double n5_cost_mod = 0.3;
  double n6_cost_mod = 0.35;
  double n7_cost_mod = 0.4;
  double n8_cost_mod = 0.45;
  double n9_cost_mod = 0.5;


  // Now we make two suppliers: one for eggs, the other for spam, and draw arcs
  // between the appropritae supplier and requester.
  ExchangeNodeGroup::Ptr s(new ExchangeNodeGroup);
  ExchangeNode::Ptr egg_supply(new ExchangeNode());
  ExchangeNode::Ptr spam_supply(new ExchangeNode());
  s->AddExchangeNode(egg_supply);
  s->AddExchangeNode(spam_supply);
  g.AddSupplyGroup(s);

  // The prices egg_supply and spam_supply will want to charge for their goods
  double egg_cost = 2.30;
  double spam_cost = 4.25;

  Arc arc_1_eggs(n1, egg_supply);
  Arc arc_2_eggs(n2, egg_supply);
  Arc arc_3_eggs(n3, egg_supply);
  Arc arc_4_eggs(n4, egg_supply);
  Arc arc_5_eggs(n4, egg_supply);
  Arc arc_6_spam(n6, spam_supply);
  Arc arc_7_spam(n7, spam_supply);
  Arc arc_8_spam(n8, spam_supply);
  Arc arc_9_spam(n9, spam_supply);

  // These weights will determine the node_weight using the following formula:
  //    node_weight = commod_weight * (1 + avg_arc_cost) / (1 + |avg_arc_cost|)
  // which for all our arcs will just be:
  //    node_weight = commod_weight
  // This is used to sort the groups by their average node_weight later.
  std::map<std::string, double> weights;
  weights["spam"] = 5.0;
  weights["eggs"] = 2.0;

  // Create the Greedy Preconditioner object with the weights
  GreedyPreconditioner gp(weights);

  // Set arc costs - this is what AvgCost reads.

  // AvgNodeWeight = 2.0
  arc_1_eggs.arc_cost(egg_cost - n1_cost_mod);  // 2.20
  arc_3_eggs.arc_cost(egg_cost - n3_cost_mod);  // 2.10
  arc_5_eggs.arc_cost(egg_cost - n5_cost_mod);  // 2.00

  // AvgNodeWeight = 4.25
  arc_2_eggs.arc_cost(egg_cost - n2_cost_mod);  // 2.15
  arc_7_spam.arc_cost(spam_cost - n7_cost_mod); // 3.85
  arc_8_spam.arc_cost(spam_cost - n8_cost_mod); // 3.80
  arc_9_spam.arc_cost(spam_cost - n9_cost_mod); // 3.75

  // AvgNodeWeight = 3.5
  arc_4_eggs.arc_cost(egg_cost - n4_cost_mod);  // 2.05
  arc_6_spam.arc_cost(spam_cost - n6_cost_mod); // 3.90

  // add all the arcs to the graph
  g.AddArc(arc_1_eggs);
  g.AddArc(arc_2_eggs);
  g.AddArc(arc_3_eggs);
  g.AddArc(arc_4_eggs);
  g.AddArc(arc_5_eggs);
  g.AddArc(arc_6_spam);
  g.AddArc(arc_7_spam);
  g.AddArc(arc_8_spam);
  g.AddArc(arc_9_spam);

  // initial state
  EXPECT_EQ(g.request_groups().at(0), g1);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(0), n1);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(1), n3);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(2), n5);

  EXPECT_EQ(g.request_groups().at(1), g2);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(0), n2);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(1), n7);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(0), n8);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(1), n9);

  EXPECT_EQ(g.request_groups().at(2), g3);
  EXPECT_EQ(g.request_groups().at(2)->nodes().at(0), n4);
  EXPECT_EQ(g.request_groups().at(2)->nodes().at(1), n6);

  std::map<ExchangeNode::Ptr, double> avg_costs;

  // Get the AvgCost for each node on the graph
  avg_costs[n1] = AvgCost(n1, &g); // 2.20
  avg_costs[n2] = AvgCost(n2, &g); // 2.15
  avg_costs[n3] = AvgCost(n3, &g); // 2.10
  avg_costs[n4] = AvgCost(n4, &g); // 2.05
  avg_costs[n5] = AvgCost(n5, &g); // 2.00
  avg_costs[n6] = AvgCost(n6, &g); // 3.90
  avg_costs[n7] = AvgCost(n7, &g); // 3.85
  avg_costs[n8] = AvgCost(n8, &g); // 3.80
  avg_costs[n9] = AvgCost(n9, &g); // 3.75

  // The node weight asociated with each group here is just the commod_weight
  double exp_nw_1 = weights[n1->commod]; 
  double exp_nw_2 = weights[n2->commod];
  double exp_nw_3 = weights[n3->commod];
  double exp_nw_4 = weights[n4->commod];
  double exp_nw_5 = weights[n5->commod];
  double exp_nw_6 = weights[n6->commod];
  double exp_nw_7 = weights[n7->commod];
  double exp_nw_8 = weights[n8->commod];
  double exp_nw_9 = weights[n9->commod];

  EXPECT_DOUBLE_EQ(NodeWeight(n1, &weights, avg_costs[n1]), exp_nw_1);
  EXPECT_DOUBLE_EQ(NodeWeight(n2, &weights, avg_costs[n2]), exp_nw_2);
  EXPECT_DOUBLE_EQ(NodeWeight(n3, &weights, avg_costs[n3]), exp_nw_3);
  EXPECT_DOUBLE_EQ(NodeWeight(n4, &weights, avg_costs[n4]), exp_nw_4);
  EXPECT_DOUBLE_EQ(NodeWeight(n5, &weights, avg_costs[n5]), exp_nw_5);
  EXPECT_DOUBLE_EQ(NodeWeight(n6, &weights, avg_costs[n6]), exp_nw_6);
  EXPECT_DOUBLE_EQ(NodeWeight(n7, &weights, avg_costs[n7]), exp_nw_7);
  EXPECT_DOUBLE_EQ(NodeWeight(n8, &weights, avg_costs[n8]), exp_nw_8);
  EXPECT_DOUBLE_EQ(NodeWeight(n9, &weights, avg_costs[n9]), exp_nw_9);

  double exp_group_weight_1 = (exp_nw_1 + exp_nw_3 + exp_nw_5) / 3;
  double exp_group_weight_2 = (exp_nw_2 + exp_nw_7 + exp_nw_8 + exp_nw_9) / 4;
  double exp_group_weight_3 = (exp_nw_4 + exp_nw_6) / 2;
  
  EXPECT_DOUBLE_EQ(GroupWeight(g1, &weights, &avg_costs), exp_group_weight_1);
  EXPECT_DOUBLE_EQ(GroupWeight(g2, &weights, &avg_costs), exp_group_weight_2);
  EXPECT_DOUBLE_EQ(GroupWeight(g3, &weights, &avg_costs), exp_group_weight_3);

  // Sort the nodes and groups based on their GroupWeights and AvgCosts
  gp.Condition(&g);

  // final state
  EXPECT_EQ(g.request_groups().at(0), g1);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(0), n5);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(1), n3);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(2), n1);

  EXPECT_EQ(g.request_groups().at(1), g3);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(0), n4);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(1), n6);

  EXPECT_EQ(g.request_groups().at(2), g2);
  EXPECT_EQ(g.request_groups().at(2)->nodes().at(0), n2);
  EXPECT_EQ(g.request_groups().at(2)->nodes().at(1), n9);
  EXPECT_EQ(g.request_groups().at(2)->nodes().at(0), n8);
  EXPECT_EQ(g.request_groups().at(2)->nodes().at(1), n7);
}
