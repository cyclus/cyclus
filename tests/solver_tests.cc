#include <gtest/gtest.h>

#include "solver_tests.h"

namespace cyclus {

template <class T>
SolverCase* CreateSolverCase();

template <>
SolverCase* CreateSolverCase<Case0>() {
  return new Case0;
}

// template <>
// SolverCase* CreateSolverCase<Case1a>() {
//   return new Case1a();
// }

// template <>
// SolverCase* CreateSolverCase<Case1b>() {
//   return new Case1b();
// }

// Then we define a test fixture class template.
template <class T>
class ExchangeSolverTest : public testing::Test {
 protected:
  // The ctor calls the factory function to create a prime table
  // implemented by T.
  ExchangeSolverTest() : case_(CreateSolverCase<T>()) {  }

  virtual ~ExchangeSolverTest() { delete case_; }
  
  // Note that we test an implementation via the base interface
  // instead of the actual implementation class.  This is important
  // for keeping the tests close to the real world scenario, where the
  // implementation is invoked via the base interface.  It avoids
  // got-yas where the implementation class has a method that shadows
  // a method with the same name (but slightly different argument
  // types) in the base interface, for example.
  SolverCase* const case_;
};

#if GTEST_HAS_TYPED_TEST

using testing::Types;

// Google Test offers two ways for reusing tests for different types.
// The first is called "typed tests".  You should use it if you
// already know *all* the types you are gonna exercise when you write
// the tests.

// To write a typed test case, first use
//
//   TYPED_TEST_CASE(TestCaseName, TypeList);
//
// to declare it and specify the type parameters.  As with TEST_F,
// TestCaseName must match the test fixture name.

// The list of types we want to test.
// typedef Types<Case0, Case1a, Case1b> Implementations;
typedef Types<Case0> Implementations;

TYPED_TEST_CASE(ExchangeSolverTest, Implementations);

// Then use TYPED_TEST(TestCaseName, TestName) to define a typed test,
// similar to TEST_F.
TYPED_TEST(ExchangeSolverTest, TestCase) {
  // // Inside the test body, you can refer to the type parameter by
  // // TypeParam, and refer to the fixture class by TestFixture.  We
  // // don't need them in this example.

  // SolverFactory fac;
  // boost::shared_ptr<ExchangeSolver> solver;
  // std::vector<std::string> types = fac.solver_types();

  // std::vector<std::string>::iterator it;
  // for (it = types.begin(); it != types.end(); ++it) {
  //   ExchangeGraph g;
  //   this->case_->Construct(&g);
  //   solver = fac.Solver(*it, &g);
  //   solver->Solve();
  //   this->case_->Test(*it, &g);
  //   // g.reset();
  // }
}

#endif  // GTEST_HAS_TYPED_TEST

} // namespace cyclus
