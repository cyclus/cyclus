#include <gtest/gtest.h>

#include "exchange_graph.h"

#include "exchange_solver.h"

using cyclus::ExchangeSolver;
using cyclus::ExchangeGraph;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MockSolver: public ExchangeSolver {
 public:
  explicit MockSolver(ExchangeGraph& g) : ExchangeSolver(g), i(0) { }
  
  virtual void Solve() { ++i; }

  int i;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExSolverTests, Interface) {
  cyclus::ExchangeGraph g;
  MockSolver s(g);
  EXPECT_EQ(0, s.i);
  s.Solve();
  EXPECT_EQ(1, s.i);
  s.Solve();
  EXPECT_EQ(2, s.i);
}
