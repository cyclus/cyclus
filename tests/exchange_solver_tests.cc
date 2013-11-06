#include <gtest/gtest.h>

#include "exchange_graph.h"

#include "exchange_solver.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MockSolver: public cyclus::ExchangeSolver {
 public:
  MockSolver() : i(0) { }
  
  virtual void Solve(cyclus::ExchangeGraph& g) { ++i; }

  int i;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExSolverTests, Interface) {
  cyclus::ExchangeGraph g;
  MockSolver s;
  EXPECT_EQ(0, s.i);
  s.Solve(g);
  EXPECT_EQ(1, s.i);
  s.Solve(g);
  EXPECT_EQ(2, s.i);
}
