#include <gtest/gtest.h>

#include "exchange_solver.h"

using cyclus::ExchangeSolver;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MockSolver: public ExchangeSolver {
 public:
  explicit MockSolver() : i(0) {}

  virtual void Solve() { ++i; }

  int i;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(ExSolverTests, Interface) {
  MockSolver s;
  EXPECT_EQ(0, s.i);
  s.Solve();
  EXPECT_EQ(1, s.i);
  s.Solve();
  EXPECT_EQ(2, s.i);
}
