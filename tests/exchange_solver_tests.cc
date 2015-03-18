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

ExchangeGraph* gen(bool consumer_lteq) {
  ExchangeNode::Ptr u(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());
  Arc a(u, v);

  u->unit_capacities[a].push_back(1);
  u->prefs[a] = 1;

  RequestGroup::Ptr gu(new RequestGroup(1));
  gu->AddExchangeNode(u);
  gu->AddCapacity(1);
  ExchangeNodeGroup::Ptr gv(new ExchangeNodeGroup());
  gv->AddExchangeNode(v);

  if (consumer_lteq) {
    u->unit_capacities[a].push_back(1);
    gu->AddCapacity(0.5, LTEQ);
  } else {
    v->unit_capacities[a].push_back(1);
    gv->AddCapacity(0.5);
  }
  
  ExchangeGraph* g = new ExchangeGraph();
  g->AddRequestGroup(gu);
  g->AddSupplyGroup(gv);
  g->AddArc(a);
  return g;
}

TEST(ExSolverTests, CustomConstraintsGreedy) {
  GreedySolver greedy(false);
  bool consumer_lteq;
  
  // test supplier and consumer constraints
  consumer_lteq = false;
  ExchangeGraph* g = gen(consumer_lteq);
  Match exp_g = Match(g->arcs().at(0), 0.5);
  greedy.graph(g);
  greedy.Solve();
  ASSERT_TRUE(g->matches().size() > 0);
  EXPECT_EQ(exp_g, g->matches().at(0));
  delete g;

  // test consumer with LTEQ constraints
  consumer_lteq = true;
  ExchangeGraph* h = gen(consumer_lteq);
  Match exp_h = Match(h->arcs().at(0), 0.5);
  greedy.graph(h);
  greedy.Solve();
  ASSERT_TRUE(h->matches().size() > 0);
  EXPECT_EQ(exp_h, h->matches().at(0));
  delete h;  
}

TEST(ExSolverTests, CustomConstraintsProg) {
  ProgSolver cbc("cbc");
  bool consumer_lteq;
  
  // test supplier and consumer constraints
  consumer_lteq = false;
  ExchangeGraph* g = gen(consumer_lteq);
  Match exp_g = Match(g->arcs().at(0), 0.5);
  cbc.graph(g);
  cbc.Solve();
  ASSERT_TRUE(g->matches().size() > 0);
  EXPECT_EQ(exp_g, g->matches().at(0));
  g->ClearMatches();
  delete g;

  // test consumer with LTEQ constraints
  consumer_lteq = true;
  ExchangeGraph* h = gen(consumer_lteq);
  Match exp_h = Match(h->arcs().at(0), 0.5);
  cbc.graph(h);
  cbc.Solve();
  ASSERT_TRUE(h->matches().size() > 0);
  EXPECT_EQ(exp_h, h->matches().at(0));
  h->ClearMatches();
  delete h;  
}

}
