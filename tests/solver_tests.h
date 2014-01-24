#ifndef CYCLUS_TESTS_SOLVER_TESTS_H_
#define CYCLUS_TESTS_SOLVER_TESTS_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "exchange_test_cases.h"

namespace cyclus {

/// This testing suite uses <a
/// href="http://code.google.com/p/googletest/wiki/V1_7_AdvancedGuide#Type-Parameterized_Tests">type-parameterized
/// tests</a>. See an example by google <a
/// href="http://code.google.com/p/googletest/source/browse/trunk/samples/sample6_unittest.cc">here</a>.
///
/// To add a new case, add the appropriate factory method. For some new test
/// case, CaseN, you would add (in this header file):
///
///   @code
///   template <>
///   ExchangeCase* CreateExchangeCase<Case0>() { return new CaseN; }
///   @endcode
///
/// and also add your new case to Implementations in the implementation file.
///
/// To add a new solver, add the appropriate TYPED_TEST in the implementation
/// file. For some new solver, MySolver, you would add:
///
///   @code
///   TYPED_TEST(ExchangeSolverTest, MySolver) {
///     std::string type = "greedy";
///     ExchangeGraph g;
///     this->case_->Construct(&g);
///     MySolver solver(&g);
///     solver.Solve();
///     this->case_->Test(type, &g);
///   }
///   @endcode
///
/// See exchange_test_cases.h for instructions on adding solutions for your
/// solver.
///
template <class T>
ExchangeCase* CreateExchangeCase();

template <>
ExchangeCase* CreateExchangeCase<Case0>() { return new Case0; }

template <>
ExchangeCase* CreateExchangeCase<Case1a>() { return new Case1a; }

template <>
ExchangeCase* CreateExchangeCase<Case1b>() { return new Case1b; }

/// ExchangeSolverTest fixture class template.
template <class T>
class ExchangeSolverTest : public testing::Test {
 protected:
  ExchangeSolverTest() : case_(CreateExchangeCase<T>()) {}
  virtual ~ExchangeSolverTest() { delete case_; }
  ExchangeCase* const case_;
};

} // namespace cyclus

#endif // ifndef CYCLUS_TESTS_SOLVER_TESTS_H_
