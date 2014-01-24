#include <gtest/gtest.h>

#include "exchange_test_cases.h"
#include "solver_tests.h"

namespace cyclus {

template <class T>
ExchangeCase* CreateExchangeCase();

template <>
ExchangeCase* CreateExchangeCase<Case0>() {
  return new Case0;
}

template <>
ExchangeCase* CreateExchangeCase<Case1a>() {
  return new Case1a;
}

template <>
ExchangeCase* CreateExchangeCase<Case1b>() {
  return new Case1b;
}

// Then we define a test fixture class template.
template <class T>
class ExchangeSolverTest : public testing::Test {
 protected:
  ExchangeSolverTest() : case_(CreateExchangeCase<T>()) {  }

  virtual ~ExchangeSolverTest() { delete case_; }
  
  ExchangeCase* const case_;
};

#if GTEST_HAS_TYPED_TEST

using testing::Types;

// The list of types we want to test.
typedef Types<Case0, Case1a, Case1b> Implementations;

TYPED_TEST_CASE(ExchangeSolverTest, Implementations);

TYPED_TEST(ExchangeSolverTest, GreedySolver) {
  SolverFactory fac;
  boost::shared_ptr<ExchangeSolver> solver;
  std::vector<std::string> types = fac.solver_types();

  std::vector<std::string>::iterator it;
  for (it = types.begin(); it != types.end(); ++it) {
    ExchangeGraph g;
    this->case_->Construct(&g);
    solver = fac.Solver(*it, &g);
    solver->Solve();
    this->case_->Test(*it, &g);
    // g.reset();
  }
}

#endif  // GTEST_HAS_TYPED_TEST

} // namespace cyclus
