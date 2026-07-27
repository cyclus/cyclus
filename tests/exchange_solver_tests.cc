#include <gtest/gtest.h>
#include <limits>
#include <vector>

#include "exchange_solver.h"
#include "error.h"
#include "exchange_graph.h"

using cyclus::ExchangeSolver;
using cyclus::Arc;
using cyclus::ExchangeGraph;
using cyclus::ExchangeNode;
using cyclus::ValueError;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MockSolver : public ExchangeSolver {
 public:
  explicit MockSolver(bool exclusive_orders = kDefaultExclusive)
      : ExchangeSolver(exclusive_orders), i(0) {}

  virtual double SolveGraph() {
    ++i;
    return 0;
  }

  int i;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class PseudoCostTests : public ::testing::Test {
 protected:
  virtual void SetUp() {
    solver_.graph(&graph_);
  }

  void AddArc(double cost) {
    ExchangeNode::Ptr req(new ExchangeNode(1.0));
    ExchangeNode::Ptr bid(new ExchangeNode(1.0));
    nodes_.push_back(req);
    nodes_.push_back(bid);

    Arc arc(req, bid);
    arc.arc_cost(cost);
    graph_.AddArc(arc);
  }

  ExchangeGraph graph_;
  MockSolver solver_;
  std::vector<ExchangeNode::Ptr> nodes_;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(PseudoCostTests, UsesLargestPositiveCost) {
  AddArc(2.0);
  AddArc(5.0);

  EXPECT_DOUBLE_EQ(5.5, solver_.PseudoCostByArcCost(0.1));
  EXPECT_DOUBLE_EQ(5.5, solver_.PseudoCost());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(PseudoCostTests, IsStrictlyGreaterThanZeroCost) {
  AddArc(0.0);

  double pseudo_cost = solver_.PseudoCostByArcCost(0.1);
  EXPECT_DOUBLE_EQ(0.1, pseudo_cost);
  EXPECT_GT(pseudo_cost, 0.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(PseudoCostTests, HandlesNegativeCosts) {
  AddArc(-10.0);
  AddArc(-2.0);

  double pseudo_cost = solver_.PseudoCostByArcCost(0.1);
  EXPECT_DOUBLE_EQ(-1.8, pseudo_cost);
  EXPECT_GT(pseudo_cost, -10.0);
  EXPECT_GT(pseudo_cost, -2.0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(PseudoCostTests, UsesEffectiveExclusiveCostOnce) {
  ExchangeNode::Ptr req(new ExchangeNode(0.5, true));
  ExchangeNode::Ptr bid(new ExchangeNode(1.0));
  nodes_.push_back(req);
  nodes_.push_back(bid);

  Arc arc(req, bid);
  arc.arc_cost(5.0);
  ASSERT_DOUBLE_EQ(0.5, arc.excl_val());
  graph_.AddArc(arc);

  // The effective cost is 5.0 / 0.5 = 10.0, so the pseudo cost is 11.0.
  EXPECT_DOUBLE_EQ(11.0, solver_.PseudoCostByArcCost(0.1));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(PseudoCostTests, ReturnsZeroForEmptyGraph) {
  EXPECT_DOUBLE_EQ(0.0, solver_.PseudoCostByArcCost(0.1));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(PseudoCostTests, RejectsNonFiniteArcCosts) {
  const double bad_costs[] = {
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::infinity(),
      -std::numeric_limits<double>::infinity(),
  };

  for (int i = 0; i < 3; ++i) {
    ExchangeGraph graph;
    MockSolver solver;
    solver.graph(&graph);

    ExchangeNode::Ptr req(new ExchangeNode(1.0));
    ExchangeNode::Ptr bid(new ExchangeNode(1.0));
    Arc arc(req, bid);
    arc.arc_cost(bad_costs[i]);
    graph.AddArc(arc);

    EXPECT_THROW(solver.PseudoCostByArcCost(0.1), ValueError);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExSolverTests, Interface) {
  MockSolver s;
  EXPECT_EQ(0, s.i);
  s.Solve();
  EXPECT_EQ(1, s.i);
  s.Solve();
  EXPECT_EQ(2, s.i);
}
