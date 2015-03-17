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

ExchangeGraph* gen() {
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
  gu->AddCapacity(1, LTEQ);
  ExchangeNodeGroup::Ptr gv(new ExchangeNodeGroup());
  gv->AddExchangeNode(v);
  gv->AddCapacity(2); // 2 > 1

  ExchangeGraph* g = new ExchangeGraph();
  g->AddRequestGroup(gu);
  g->AddSupplyGroup(gv);
  g->AddArc(a);
  return g;
}

TEST(ExSolverTests, Constraints) {
  ExchangeGraph* g = gen();
  ProgSolver s("clp");
  // GreedySolver s(false);

  // test unit flow to consumer
  s.graph(g);
  s.Solve();
  Match exp = Match(g->arcs().at(0), 1);
  ASSERT_TRUE(g->matches().size() > 0);
  EXPECT_EQ(exp, g->matches().at(0));
  g->ClearMatches();

  // test less-than unit flow to consumer
  RequestGroup::Ptr rg = g->request_groups()[0];
  rg->capacities()[1] = 0.5;
  EXPECT_EQ(0.5, g->request_groups().at(0)->capacities.at(1));
  s.Solve();
  Match exp2 = Match(g->arcs().at(0), 0.5);
  ASSERT_TRUE(g->matches().size() > 0);
  EXPECT_EQ(exp2, g->matches().at(0));
  
  delete g;
}

}
