#include <gtest/gtest.h>

#include "greedy_solver_tests.h"

using cyclus::ExchangeGraph;
using cyclus::GreedySolver;
using cyclus::Node;
using cyclus::NodeSet;
using cyclus::RequestSet;
using cyclus::Match;

/* --- */
// Case 0:
// 0 nodes
TEST(GrSolverTests, Case0) {
  ExchangeGraph g;
  GreedySolver solver(g);
  solver.Solve();
  EXPECT_TRUE(g.matches.empty());
}
/* --- */

/* --- */
// Case 1:
// 1 node
TEST(GrSolverTests, Case1a) {
  // 1 request node
  ExchangeGraph g = SetUp1a();
  GreedySolver solver(g);
  solver.Solve();
  EXPECT_TRUE(g.matches.empty());
}

TEST(GrSolverTests, Case1b) {
// 1 supply node
  ExchangeGraph g = SetUp1b();
  GreedySolver solver(g);
  solver.Solve();
  EXPECT_TRUE(g.matches.empty());
}
/* --- */

/* --- */
// Case 2:
// 1 supply node with capacity, cap
// 1 request node with request quantity, qty
TEST(GrSolverTests, Case2a) {
  // qty < cap
  double qty = 5;
  double cap = 10;
  ExchangeGraph g = SetUp2(qty, cap);
  GreedySolver solver(g);
  solver.Solve();
  
  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], qty);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2b) {
  // qty = cap
  double qty = 5;
  double cap = 10;
  ExchangeGraph g = SetUp2(qty, cap);
  GreedySolver solver(g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], qty);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2c) {
  // qty > cap
  double qty = 10;
  double cap = 5;
  ExchangeGraph g = SetUp2(qty, cap);
  GreedySolver solver(g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], cap);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}
/* --- */
