#ifndef CYCLUS_TESTS_SOLVER_TESTS_H_
#define CYCLUS_TESTS_SOLVER_TESTS_H_

#include <string>

namespace cyclus {

/// This testing suite uses <a
/// href="http://code.google.com/p/googletest/wiki/V1_7_AdvancedGuide#Typed_Tests">typed
/// tests</a>. See an example by google <a
/// href="http://code.google.com/p/googletest/source/browse/trunk/samples/sample6_unittest.cc">here</a>.
///
/// To add a new test case, first add the appropriate class to
/// exchange_test_cases.h. Next, add the appropriate factory method. For some
/// new test case, CaseN, you would add (in the solver_tests implementation
/// file):
///
///   @code
///   template <>
///   ExchangeCase* CreateExchangeCase<CaseN>() { return new CaseN; }
///   @endcode
///
/// Finally, add your new case to Implementations in the implementation file.
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

class ExchangeCase;

template <class T>
ExchangeCase* CreateExchangeCase();

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
