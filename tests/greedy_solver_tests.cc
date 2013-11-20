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
// 1 supply node with capacity, c, and unit capacity, u_s
// 1 request node with request quantity, q, and unit capacity, u_r
// flow from s->r = f
TEST(GrSolverTests, Case2a) {
  // q < c, u_s = 1
  // f = q
  double q = 5;
  double u_r = 1;
  double c= 10;
  double u_s = 1;
  double f = q;
  
  ExchangeGraph g = SetUp2(q, u_r, c, u_s);
  GreedySolver solver(&g);
  solver.Solve();
  
  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], f);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2b) {
  // q = c, u_s = 1
  // f = c
  double q = 5;
  double u_r = 1;
  double c = 5;
  double u_s = 1;
  double f = c;
  
  ExchangeGraph g = SetUp2(q, u_r, c, u_s);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], f);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2c) {
  // q > c, u_s = 1, u_r = 1
  // f = c
  double q = 10;
  double u_r = 1;
  double c = 5;
  double u_s = 1;
  double f = c;
  
  ExchangeGraph g = SetUp2(q, u_r, c, u_s);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], f);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2d) {
  // q > c, u_s < 1, c/u_s > q, u_r = 1
  // f = q
  double q = 10;
  double u_r = 1;
  double c = 5;
  double u_s = 0.3;
  double f = q;
  
  ExchangeGraph g = SetUp2(q, u_r, c, u_s);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], f);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2e) {
  // q = c, u_s < 1 ( => c/u_s > q ), u_r = 1
  // f = q
  double q = 10;
  double u_r = 1;
  double c = 5;
  double u_s = 0.3;
  double f = q;

  ExchangeGraph g = SetUp2(q, u_r, c, u_s);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], f);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2f) {
  // q = c, u_s > 1 ( => c/u_s < q ), u_r = 1
  // f = c / u_s
  double q = 10;
  double u_r = 1;
  double c = 10;
  double u_s = 2;
  double f = c / u_s;
  
  ExchangeGraph g = SetUp2(q, u_r, c, u_s);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], f);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2g) {
  // q = c, u_s = 1, u_r < 1 ( => q/u_r > c )
  // f = c
  double q = 10;
  double u_r = 0.9;
  double c = 10;
  double u_s = 1;
  double f = c;
  
  ExchangeGraph g = SetUp2(q, u_r, c, u_s);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], f);
  ASSERT_TRUE(g.matches.size() > 0);
  EXPECT_EQ(exp, g.matches[0]);
}

TEST(GrSolverTests, Case2h) {
  // q = c, u_s = 1, u_r > 1 ( => q/u_r < c )
  // f = q / u_r
  double q = 10;
  double u_r = 2;
  double c = 10;
  double u_s = 1;
  double f = q / u_r;
  
  ExchangeGraph g = SetUp2(q, u_r, c, u_s);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  Match exp = Match(g.arcs_[0], f);
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
  // q = c1
  // f1 = c1, f2 DNE
  double q = 5;
  double c1 = 5;
  double c2 = 10;
  double f1 = c1;
  
  ExchangeGraph g = SetUp3(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();
  
  ASSERT_TRUE(g.arcs_.size() > 0);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp = Match(g.arcs_[0], f1);
  Match arr[] = {exp};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case3b) {
  // q - c1 < c2
  // f1 = c1, f2 = q - c1
  double q = 5;
  double c1 = 3;
  double c2 = q - c1 + 0.1;
  double f1 = c1;
  double f2 = q - c1;
  
  ExchangeGraph g = SetUp3(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], f1);
  Match exp2 = Match(g.arcs_[1], f2);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case3c) {
  // q - c1 = c2
  // f1 = c1, f2 = c2
  double q = 5;
  double c1 = 3;
  double c2 = q - c1;
  double f1 = c1;
  double f2 = c2;

  ExchangeGraph g = SetUp3(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();
  
  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], f1);
  Match exp2 = Match(g.arcs_[1], f2);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case3d) {
  // q - c1 > c2
  // f1 = c1, f2 = c2
  double q = 5;
  double c1 = 3;
  double c2 = q - c1 - 0.1;
  double f1 = c1;
  double f2 = c2;
  
  ExchangeGraph g = SetUp3(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();
    
  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], f1);
  Match exp2 = Match(g.arcs_[1], f2);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}
/* --- */

/* --- */
// Case 4:
// 1 suppliers (2 nodes) with capacity, c
// 2 requester (2 nodes) with request quantities, q1 & q2
// flow from s -> r1 := f1
// flow from s -> r2 := f2
TEST(GrSolverTests, Case4a) {
  // q1 > c
  // f1 = c, f2 DNE
  double q1 = 5;
  double q2 = 1;
  double c = 1;
  double f1 = c;
  
  ExchangeGraph g = SetUp4(q1, q2, c);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp = Match(g.arcs_[0], f1);
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
  double f1 = c;

  ExchangeGraph g = SetUp4(q1, q2, c);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 0);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp = Match(g.arcs_[0], f1);
  Match arr[] = {exp};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case4c) {
  // q1 < c, q2 > c - q1
  // f1 = q1, f2 = c - q1
  double q1 = 3;
  double c = 5;
  double q2 = c - q1 + 0.1;
  double f1 = q1;
  double f2 = c - q1;
  
  ExchangeGraph g = SetUp4(q1, q2, c);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], f1);
  Match exp2 = Match(g.arcs_[1], f2);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case4d) {
  // q1 < c, q2 = c - q1
  // f1 = q1, f2 = c - q1
  double q1 = 2;
  double c = 5;
  double q2 = c - q1;
  double f1 = q1;
  double f2 = c - q1;
  
  ExchangeGraph g = SetUp4(q1, q2, c);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], f1);
  Match exp2 = Match(g.arcs_[1], f2);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case4e) {
  // q1 < c, q2 < c - q1
  // f1 = q1, f2 = q2
  double q1 = 3;
  double c = 8;
  double q2 = c - q1 - 0.1;
  double f1 = q1;
  double f2 = q2;
  
  ExchangeGraph g = SetUp4(q1, q2, c);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], f1);
  Match exp2 = Match(g.arcs_[1], f2);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}
/* --- */

/* --- */
// Case 5:
// 2 suppliers (2 nodes) with capacitoes, c1 & c2
// 1 requester (2 nodes) with request quantities, q
// flow from s1 -> r := f1
// flow from s2 -> r := f2
TEST(GrSolverTests, Case5a) {
  // q < c1
  // f1 = q, f2 DNE
  double q = 1;
  double c1 = 5;
  double c2 = 0.5;
  double f1 = q;

  ExchangeGraph g = SetUp5(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp = Match(g.arcs_[0], f1);
  Match arr[] = {exp};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case5b) {
  // q = c1
  // f1 = c1, f2 DNE
  double q = 5;
  double c1 = 5;
  double c2 = 0.5;
  double f1 = c1;

  ExchangeGraph g = SetUp5(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp = Match(g.arcs_[0], f1);
  Match arr[] = {exp};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case5c) {
  // q > c1, c2 > q - c1
  // f1 = c1, f2 = q - c1
  double q = 7;
  double c1 = 5;
  double c2 = q - c1 + 1;
  double f1 = c1;
  double f2 = q - c1;

  ExchangeGraph g = SetUp5(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], f1);
  Match exp2 = Match(g.arcs_[1], f2);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case5d) {
  // q > c1, c2 = q - c1
  // f1 = c1, f2 = c2
  double q = 7;
  double c1 = 5;
  double c2 = q - c1;
  double f1 = c1;
  double f2 = c2;

  ExchangeGraph g = SetUp5(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], f1);
  Match exp2 = Match(g.arcs_[1], f2);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case5e) {
  // q > c1, c2 < q - c1
  // f1 = c1, f2 = c2
  double q = 7;
  double c1 = 5;
  double c2 = q - c1 - 1;
  double f1 = c1;
  double f2 = c2;

  ExchangeGraph g = SetUp5(q, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 1);
  EXPECT_EQ(g.arcs_.size(), 2);
  
  Match exp1 = Match(g.arcs_[0], f1);
  Match exp2 = Match(g.arcs_[1], f2);
  Match arr[] = {exp1, exp2};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}
/* --- */

/* --- */
// Case 6:
// 2 suppliers (2 nodes ea.) with capacitoes, c1 & c2
// 2 requester (2 nodes eq.) with request quantities, q1 & q2
// flow from s1 -> r1 := f1
// flow from s2 -> r1 := f2
// flow from s1 -> r2 := f3
// flow from s2 -> r2 := f4
TEST(GrSolverTests, Case6a) {
  // q1 > c1, c2 > q1 - c1, q2 < c2 - (q1 - c1)
  // f1 = c1, f2 = q1 - c1, f3 DNE, f4 = q2
  double q1 = 7;
  double c1 = 5;
  double c2 = q1 - c1 + 3;
  double q2 = c2 - (q1 - c1) - 1;
  double f1 = c1;
  double f2 = q1 - c1;
  double f4 = q2;

  ExchangeGraph g = SetUp6(q1, q2, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 3);
  EXPECT_EQ(g.arcs_.size(), 4);
  
  Match exp1 = Match(g.arcs_[0], f1);
  Match exp2 = Match(g.arcs_[1], f2);
  Match exp3 = Match(g.arcs_[3], f4);
  Match arr[] = {exp1, exp2, exp3};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}

TEST(GrSolverTests, Case6b) {
  // q1 < c1, q2 > c1 - q1, c2 > q2 - (c1 - q1)
  // f1 = q1, f2 DNE, f3 = c1 - q1, f4 = q2 - (c1 - q1)
  double q1 = 5;
  double c1 = 7;
  double q2 = c1 - q1 + 0.5;
  double c2 = q2 - (c1 - q1) + 3;
  double f1 = q1;
  double f3 = c1 - q1;
  double f4 = q2 - (c1 - q1);

  ExchangeGraph g = SetUp6(q1, q2, c1, c2);
  GreedySolver solver(&g);
  solver.Solve();

  ASSERT_TRUE(g.arcs_.size() > 3);
  EXPECT_EQ(g.arcs_.size(), 4);
  
  Match exp1 = Match(g.arcs_[0], f1);
  Match exp2 = Match(g.arcs_[2], f3);
  Match exp3 = Match(g.arcs_[3], f4);
  Match arr[] = {exp1, exp2, exp3};
  std::vector<Match> vexp(arr, arr + sizeof(arr) / sizeof(arr[0]));
  EXPECT_EQ(vexp, g.matches);
}
/* --- */
