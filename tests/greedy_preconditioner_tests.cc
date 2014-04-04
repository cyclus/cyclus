#include <gtest/gtest.h>

#include "exchange_graph.h"

#include "greedy_preconditioner.h"

using cyclus::ExchangeNode;
using cyclus::ExchangeGraph;
using cyclus::GreedyPreconditioner;
using cyclus::RequestGroup;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ConditionerTests, Conditioning) {
  std::map<ExchangeNode::Ptr, double> avg_prefs;
  
  ExchangeNode::Ptr n11(new ExchangeNode());
  n11->commod = "eggs";
  avg_prefs[n11] = 0.25;
  ExchangeNode::Ptr n12(new ExchangeNode());
  n12->commod = "spam";
  avg_prefs[n12] = 0.75;
  ExchangeNode::Ptr n13(new ExchangeNode());
  n13->commod = "eggs";
  avg_prefs[n13] = 0.25;

  ExchangeNode::Ptr n21(new ExchangeNode());
  n21->commod = "eggs";
  avg_prefs[n21] = 1;
  ExchangeNode::Ptr n22(new ExchangeNode());
  n22->commod = "spam";
  avg_prefs[n22] = 1;

  RequestGroup::Ptr g1(new RequestGroup());
  g1->AddExchangeNode(n11);
  g1->AddExchangeNode(n12);
  g1->AddExchangeNode(n13);
  
  RequestGroup::Ptr g2(new RequestGroup());
  g2->AddExchangeNode(n21);
  g2->AddExchangeNode(n22);
  
  ExchangeGraph g;
  g.AddRequestGroup(g1);
  g.AddRequestGroup(g2);

  // initial state
  EXPECT_EQ(g.request_groups().at(0), g1);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(0), n11);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(1), n12);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(2), n13);
  EXPECT_EQ(g.request_groups().at(1), g2);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(0), n21);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(1), n22);
  
  std::map<std::string, double> weights;
  weights["spam"] = 5;
  weights["eggs"] = 2;
  GreedyPreconditioner gp(weights);

  double avg1 = 4/3;
  double avg2 = 3/2;
  
  EXPECT_DOUBLE_EQ(NodeWeight(n11, &weights, avg1), avg1 * weights[n11->commod]); 
  EXPECT_DOUBLE_EQ(NodeWeight(n12, &weights, avg1), avg1 * weights[n12->commod]); 
  EXPECT_DOUBLE_EQ(NodeWeight(n13, &weights, avg1), avg1* weights[n13->commod]); 
  EXPECT_DOUBLE_EQ(NodeWeight(n21, &weights, avg2), avg2 * weights[n21->commod]); 
  EXPECT_DOUBLE_EQ(NodeWeight(n21, &weights, avg2), avg2 * weights[n21->commod]); 
  EXPECT_DOUBLE_EQ(GroupWeight(g1, &weights, &avg_prefs), 9/3 * 4/3); 
  EXPECT_DOUBLE_EQ(GroupWeight(g2, &weights, &avg_prefs), 7/2 * 3/2);
  
  gp.Condition(&g);

  // final state
  EXPECT_EQ(g.request_groups().at(0), g2);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(0), n22);
  EXPECT_EQ(g.request_groups().at(0)->nodes().at(1), n21);
  EXPECT_EQ(g.request_groups().at(1), g1);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(0), n12);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(1), n11);
  EXPECT_EQ(g.request_groups().at(1)->nodes().at(2), n13);
}

