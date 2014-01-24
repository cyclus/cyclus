#include <gtest/gtest.h>

#include "exchange_graph.h"
#include "greedy_solver.h"

#include "exchange_test_cases.h"

#include "solver_tests.h"

namespace cyclus {

#if GTEST_HAS_TYPED_TEST

using testing::Types;

// The list of types we want to test, add yours to it!
template <>
ExchangeCase* CreateExchangeCase<Case0>() { return new Case0; }

template <>
ExchangeCase* CreateExchangeCase<Case1a>() { return new Case1a; }

template <>
ExchangeCase* CreateExchangeCase<Case1b>() { return new Case1b; }

// Add it again here
typedef Types<Case0, Case1a, Case1b> Implementations;

TYPED_TEST_CASE(ExchangeSolverTest, Implementations);

TYPED_TEST(ExchangeSolverTest, GreedySolver) {
  std::string type = "greedy";
  ExchangeGraph g;
  this->case_->Construct(&g);
  GreedySolver solver(&g);
  solver.Solve();
  this->case_->Test(type, &g);
}

// add any more solvers to test here

#endif  // GTEST_HAS_TYPED_TEST

} // namespace cyclus
