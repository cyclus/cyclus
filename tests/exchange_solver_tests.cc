#include <gtest/gtest.h>

#include "exchange_solver.h"
#include "exchange_graph.h"
#include "greedy_solver.h"
#include "prog_solver.h"

namespace cyclus {

class MockSolver: public ExchangeSolver {
 public:
  explicit MockSolver() : i(0) {}

  virtual double SolveGraph() { ++i; return 0; }

  int i;
};

TEST(ExSolverTests, Interface) {
  MockSolver s;
  EXPECT_EQ(0, s.i);
  s.Solve();
  EXPECT_EQ(1, s.i);
  s.Solve();
  EXPECT_EQ(2, s.i);
}

ExchangeGraph* gen(double cap=1) {
  ExchangeNode::Ptr u(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());
  Arc a(u, v);

  u->unit_capacities[a].push_back(1);
  u->unit_capacities[a].push_back(1);
  v->unit_capacities[a].push_back(1);
  u->prefs[a] = 1;

  RequestGroup::Ptr gu(new RequestGroup(1));
  gu->AddExchangeNode(u);
  gu->AddCapacity(1);
  gu->AddCapacity(cap, LTEQ);
  ExchangeNodeGroup::Ptr gv(new ExchangeNodeGroup());
  gv->AddExchangeNode(v);
  gv->AddCapacity(2); // 2 > 1

  ExchangeGraph* g = new ExchangeGraph();
  g->AddRequestGroup(gu);
  g->AddSupplyGroup(gv);
  g->AddArc(a);
  return g;
}

TEST(ExSolverTests, ConstraintsProg) {
  ExchangeGraph* g = gen();
  ProgSolver s("clp");

  // test unit flow to consumer
  s.graph(g);
  s.Solve();
  Match exp = Match(g->arcs().at(0), 1);
  ASSERT_TRUE(g->matches().size() > 0);
  EXPECT_EQ(exp, g->matches().at(0));
  
  delete g;
}

TEST(ExSolverTests, ConstraintsProgLT) {
  ExchangeGraph* g = gen(0.5);
  ProgSolver s("clp");
  // GreedySolver s(false);

  // test unit flow to consumer
  s.graph(g);
  s.Solve();
  Match exp = Match(g->arcs().at(0), 0.5);
  ASSERT_TRUE(g->matches().size() > 0);
  EXPECT_EQ(exp, g->matches().at(0));
  
  delete g;
}

TEST(ExSolverTests, ConstraintsGreedy) {
  ExchangeGraph* g = gen();
  GreedySolver s(false);

  // test unit flow to consumer
  s.graph(g);
  s.Solve();
  Match exp = Match(g->arcs().at(0), 1);
  ASSERT_TRUE(g->matches().size() > 0);
  EXPECT_EQ(exp, g->matches().at(0));
  
  delete g;
}

TEST(ExSolverTests, ConstraintsGreedyLT) {
  ExchangeGraph* g = gen(0.5);
  GreedySolver s(false);

  // test unit flow to consumer
  s.graph(g);
  s.Solve();
  Match exp = Match(g->arcs().at(0), 0.5);
  ASSERT_TRUE(g->matches().size() > 0);
  EXPECT_EQ(exp, g->matches().at(0));
  
  delete g;
}

}
