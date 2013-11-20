#include <gtest/gtest.h>

#include <vector>

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
  GreedySolver solver(&g);
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
  GreedySolver solver(&g);
  solver.Solve();
  EXPECT_TRUE(g.matches.empty());
}

TEST(GrSolverTests, Case1b) {
// 1 supply node
  ExchangeGraph g = SetUp1b();
  GreedySolver solver(&g);
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
  GreedySolver solver(&g);
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
  GreedySolver solver(&g);
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
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], cap);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}
/* --- */

/* --- */
// Case 3:
// 2 supply node with capacity, c1 & c2
// 1 request node with request quantity, q
// flow from s1 -> r := f1
// flow from s2 -> r := f2
TEST(GrSolverTests, Case3a) {
  // q > c1
  // f1 = c1, f2 DNE
  double q = 5;
  double c1 = 5;
  double c2 = 10;
  ExchangeGraph g = SetUp3(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();
  
  ASSERT_TRUE(g.arcs_.size() > 0);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp = Match(g.arcs_[0], c1);
  Match arr[] = {exp};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case3b) {
  // q - c1 < c2
  // f1 = c1, f2 = q - c1
  double q = 5;
  double c1 = 3;
  double c2 = 3;
  ExchangeGraph g = SetUp3(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], c1);
  Match exp2 = Match(g.arcs_[1], q - c1);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case3c) {
  // q - c1 = c2
  // f1 = c1, f2 = c2
  double q = 5;
  double c1 = 3;
  double c2 = 2;
  ExchangeGraph g = SetUp3(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();
  
  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], c1);
  Match exp2 = Match(g.arcs_[1], c2);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}


TEST(GrSolverTests, Case3d) {
  // q - c1 > c2
  // f1 = c1, f2 = c2
  double q = 5;
  double c1 = 3;
  double c2 = 1;
  ExchangeGraph g = SetUp3(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();
    
  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], c1);
  Match exp2 = Match(g.arcs_[1], c2);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}
/* --- */
