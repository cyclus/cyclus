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
// 1 supply node with capacity, c, and unit capacity, u
// 1 request node with request quantity, q
// flow from s->r = f
TEST(GrSolverTests, Case2a) {
  // q < c, u = 1
  // f = q
  double q = 5;
  double c= 10;
  double u = 1;
  ExchangeGraph g = SetUp2(q, c, u);
  GreedySolver solver(&g);
  solver.Solve();
  
  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], q);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2b) {
  // q = c, u = 1
  // f = c
  double q = 5;
  double c = 5;
  double u = 1;
  ExchangeGraph g = SetUp2(q, c, u);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], c);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2c) {
  // q > c, u = 1
  // f = c
  double q = 10;
  double c = 5;
  double u = 1;
  ExchangeGraph g = SetUp2(q, c, u);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], c);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2d) {
  // q > c, u < 1, c/u > q
  // f = q
  double q = 10;
  double c = 5;
  double u = 0.3;
  ExchangeGraph g = SetUp2(q, c, u);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], q);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2e) {
  // q = c, u < 1 ( => c/u > q )
  // f = q
  double q = 10;
  double c = 5;
  double u = 0.3;
  ExchangeGraph g = SetUp2(q, c, u);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], q);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2f) {
  // q = c, u > 1 ( => c/u < q )
  // f = c / u
  double q = 10;
  double c = 10;
  double u = 2;
  ExchangeGraph g = SetUp2(q, c, u);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], c / u);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}
/* --- */

/* --- */
// Case 3:
// 2 suppliers (2 nodes) with capacity, c1 & c2
// 1 requester (1 node) with request quantity, q
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

/* --- */
// Case 4:
// 1 suppliers (2 nodes) with capacity, c
// 2 requester2 (2 node) with request quantities, q1 & q2
// flow from s -> r1 := f1
// flow from s -> r2 := f2
TEST(GrSolverTests, Case4a) {
  // q1 > c
  // f1 = c, f2 DNE
  double q1 = 5;
  double q2 = 1;
  double c = 1;
  ExchangeGraph g = SetUp4(q1, q2, c);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp = Match(g.arcs_[0], c);
  Match arr[] = {exp};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case4b) {
  // q1 = c
  // f1 = c, f2 DNE
  double q1 = 1;
  double q2 = 3;
  double c = 1;
  ExchangeGraph g = SetUp4(q1, q2, c);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp = Match(g.arcs_[0], c);
  Match arr[] = {exp};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case4c) {
  // q1 < c, q2 > c - q1
  // f1 = q1, f2 = c - q1
  double q1 = 3;
  double q2 = 3;
  double c = 5;
  ExchangeGraph g = SetUp4(q1, q2, c);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], q1);
  Match exp2 = Match(g.arcs_[1], c - q1);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case4d) {
  // q1 < c, q2 = c - q1
  // f1 = q1, f2 = c - q1
  double q1 = 2;
  double q2 = 3;
  double c = 5;
  ExchangeGraph g = SetUp4(q1, q2, c);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], q1);
  Match exp2 = Match(g.arcs_[1], c - q1);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case4e) {
  // q1 < c, q2 < c - q1
  // f1 = q1, f2 = q2
  double q1 = 3;
  double q2 = 4;
  double c = 8;
  ExchangeGraph g = SetUp4(q1, q2, c);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], q1);
  Match exp2 = Match(g.arcs_[1], q2);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}
