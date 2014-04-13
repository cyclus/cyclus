#include <gtest/gtest.h>

#include "exchange_graph.h"

#include "greedy_preconditioner.h"
#include "greedy_solver.h"


using cyclus::Arc;
using cyclus::AvgPrefComp;
using cyclus::ExchangeNode;

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
