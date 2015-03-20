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

ExchangeGraph* gen2(bool consumer_lteq) {
  bool excl = true;
  ExchangeNode::Ptr u(new ExchangeNode(1, excl));
  ExchangeNode::Ptr v(new ExchangeNode());
  Arc a(u, v);

  ExchangeNode::Ptr u2(new ExchangeNode(1, excl));
  ExchangeNode::Ptr w(new ExchangeNode());
  Arc a2(u2, w);

  u->unit_capacities[a].push_back(1);
  u->prefs[a] = 1;
  u2->unit_capacities[a2].push_back(1);
  u2->prefs[a2] = 0.5;

  RequestGroup::Ptr gu(new RequestGroup(1));
  gu->AddExchangeNode(u);
  gu->AddExchangeNode(u2);
  // std::vector<ExchangeNode::Ptr> excl_grp;
  // excl_grp.push_back(u);
  // excl_grp.push_back(u2);
  // gu->AddExclGroup(excl_grp);
  gu->AddCapacity(1);

  if (consumer_lteq) {
    u->unit_capacities[a].push_back(2);
    u2->unit_capacities[a2].push_back(1);
    gu->AddCapacity(1.0, LTEQ);
  }  

  ExchangeNodeGroup::Ptr gv(new ExchangeNodeGroup());
  gv->AddExchangeNode(v);
  // v->unit_capacities[a].push_back(1);
  // gv->AddCapacity(1.0);
  ExchangeNodeGroup::Ptr gw(new ExchangeNodeGroup());
  gw->AddExchangeNode(w);
  // w->unit_capacities[a2].push_back(1);
  // gw->AddCapacity(1.0);
  
  ExchangeGraph* g = new ExchangeGraph();
  g->AddRequestGroup(gu);
  g->AddSupplyGroup(gv);
  g->AddSupplyGroup(gw);
  g->AddArc(a);
  g->AddArc(a2);
  return g;
}


TEST(ExSolverTests, CustomConstraintsGreedy2) {
  bool consumer_lteq;
  GreedySolver greedy_nex(false);
  GreedySolver greedy_ex(true);
  
  // test supplier and consumer constraints
  consumer_lteq = false;
  ExchangeGraph* g = gen2(consumer_lteq);
  Match exp_gnex = Match(g->arcs().at(0), 1.);
  greedy_nex.graph(g);
  greedy_nex.Solve();
  ASSERT_EQ(g->matches().size(), 1);
  EXPECT_EQ(exp_gnex, g->matches().at(0));
  g->ClearMatches();
  Match exp_gex = Match(g->arcs().at(0), 1.);
  greedy_ex.graph(g);
  greedy_ex.Solve();
  ASSERT_EQ(g->matches().size(), 1);
  EXPECT_EQ(exp_gex, g->matches().at(0));
  delete g;

  // test consumer with LTEQ constraints
  consumer_lteq = true;
  ExchangeGraph* h = gen2(consumer_lteq);
  Match exp_hnex = Match(h->arcs().at(0), 0.5);
  greedy_nex.graph(h);
  greedy_nex.Solve();
  ASSERT_EQ(h->matches().size(), 1);
  EXPECT_EQ(exp_hnex, h->matches().at(0));
  h->ClearMatches();
  Match exp_hex = Match(h->arcs().at(1), 1.);
  greedy_ex.graph(h);
  greedy_ex.Solve();
  ASSERT_EQ(h->matches().size(), 1);
  EXPECT_EQ(exp_hex, h->matches().at(0));
  delete h;
}

TEST(ExSolverTests, CustomConstraintsProg2) {
  ProgSolver cbc_nex("cbc");
  bool consumer_lteq;
  
  // test supplier and consumer constraints
  consumer_lteq = false;
  ExchangeGraph* g = gen2(consumer_lteq);
  Match exp_g = Match(g->arcs().at(0), 1.0);
  cbc_nex.graph(g);
  cbc_nex.Solve();
  ASSERT_EQ(g->matches().size(), 1);
  EXPECT_EQ(exp_g, g->matches().at(0));
  g->ClearMatches();
  delete g;

  // test consumer with LTEQ constraints
  consumer_lteq = true;
  ExchangeGraph* h = gen2(consumer_lteq);
  Match exp_h = Match(h->arcs().at(0), 0.5);
  cbc_nex.graph(h);
  cbc_nex.Solve();
  ASSERT_EQ(h->matches().size(), 1);
  EXPECT_EQ(exp_h, h->matches().at(0));
  h->ClearMatches();
  delete h;  
}

}
