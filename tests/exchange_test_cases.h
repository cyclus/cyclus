#ifndef CYCLUS_TESTS_EXCHANGE_TEST_CASES_H_
#define CYCLUS_TESTS_EXCHANGE_TEST_CASES_H_

#include <string>

namespace cyclus {

class ExchangeGraph;
  
/// ExchangeCase, a class that provides a virtual interface for constructing
/// and testing a variety of ExchangeGraph/ExchangeSolver test instances.
///
/// To add test case support of a new solver type, add to the solver_type
/// if-then statement for the test case of your choosing.
class ExchangeCase {
public:
  virtual ~ExchangeCase() {}

  /// constructs an instance of an exchange graph for the test case
  virtual void Construct(ExchangeGraph* g) = 0;

  /// Tests any supported solver_types for correctness
  virtual void Test(std::string solver_type, ExchangeGraph* g) = 0;
};

/// Case0, an empty graph
/// no matches should exist
class Case0: public ExchangeCase {
 public:
  virtual ~Case0() {}
  virtual void Construct(ExchangeGraph* g);
  virtual void Test(std::string solver_type, ExchangeGraph* g);
};

/// Case1a, a single request group
/// no matches should exist
class Case1a: public ExchangeCase {
 public:
  virtual ~Case1a() {}
  virtual void Construct(ExchangeGraph* g);
  virtual void Test(std::string solver_type, ExchangeGraph* g);
};

/// Case1b, a single supply group
/// no matches should exist
class Case1b: public ExchangeCase {
 public:
  virtual ~Case1b() {}
  virtual void Construct(ExchangeGraph* g);
  virtual void Test(std::string solver_type, ExchangeGraph* g);
};

} // namespace cyclus

#endif // ifndef CYCLUS_TESTS_EXCHANGE_TEST_CASES_H_
