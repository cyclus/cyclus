#include <gtest/gtest.h>

#include "exchange_graph.h"

#include "greedy_preconditioner.h"

using cyclus::Arc;
using cyclus::AvgPref;
using cyclus::ExchangeNode;
using cyclus::ExchangeNodeGroup;
using cyclus::ExchangeGraph;
using cyclus::GreedyPreconditioner;
using cyclus::RequestGroup;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ConditionerTests, AvgPref) {
  ExchangeNode::Ptr u1(new ExchangeNode());
  ExchangeNode::Ptr u2(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());

  Arc a(u1, v);

  u1->prefs[a] = 1;

  EXPECT_TRUE(AvgPref(u1) > AvgPref(u2));
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
  double n1epref = 1/4;
  double n1spref = 3/4;

  RequestGroup::Ptr g1(new RequestGroup());
  g1->AddExchangeNode(n11);
  g1->AddExchangeNode(n12);
  g1->AddExchangeNode(n13);
  g.AddRequestGroup(g1);

  ExchangeNode::Ptr n21(new ExchangeNode());
  n21->commod = "eggs";
  ExchangeNode::Ptr n22(new ExchangeNode());
  n22->commod = "spam";
  double n2epref = 1;
  double n2spref = 1;

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

  g.AddArc(n11e);
  g.AddArc(n12s);
  g.AddArc(n13s);
  g.AddArc(n21e);
  g.AddArc(n22s);

  n11->prefs[n11e] = n1epref;
  n12->prefs[n12s] = n1spref;
  n13->prefs[n13s] = n1spref;
  n21->prefs[n21e] = n2epref;
  n22->prefs[n22s] = n2epref;

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

  std::map<ExchangeNode::Ptr, double> avg_prefs;

  double avg1 = 5./12;
  double avg2 = 2./2;
  double c1e = (1. + n1epref / (1 + n1epref));
  double c1s = (1. + n1spref / (1 + n1spref));
  double c2e = (1. + n2epref / (1 + n2epref));
  double c2s = (1. + n2spref / (1 + n2spref));
  avg_prefs[n11] = AvgPref(n11);
  avg_prefs[n12] = AvgPref(n12);
  avg_prefs[n13] = AvgPref(n13);
  avg_prefs[n21] = AvgPref(n21);
  avg_prefs[n22] = AvgPref(n22);

  double exp11 = c1e * weights[n11->commod];
  double exp12 = c1s * weights[n12->commod];
  double exp13 = c1s * weights[n13->commod];
  double exp21 = c2e * weights[n21->commod];
  double exp22 = c2s * weights[n22->commod];
  EXPECT_DOUBLE_EQ(NodeWeight(n11, &weights, avg_prefs[n11]), exp11);
  EXPECT_DOUBLE_EQ(NodeWeight(n12, &weights, avg_prefs[n12]), exp12);
  EXPECT_DOUBLE_EQ(NodeWeight(n13, &weights, avg_prefs[n13]), exp13);
  EXPECT_DOUBLE_EQ(NodeWeight(n21, &weights, avg_prefs[n21]), exp21);
  EXPECT_DOUBLE_EQ(NodeWeight(n22, &weights, avg_prefs[n22]), exp22);

  double expg1 = (exp11 + exp12 + exp13) / 3;
  double expg2 = (exp21 + exp22) / 2;
  EXPECT_DOUBLE_EQ(GroupWeight(g1, &weights, &avg_prefs), expg1);
  EXPECT_DOUBLE_EQ(GroupWeight(g2, &weights, &avg_prefs), expg2);

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
