#ifndef CYCLUS_TESTS_EXCHANGE_TEST_CASES_H_
#define CYCLUS_TESTS_EXCHANGE_TEST_CASES_H_

#include <string>

#include "exchange_graph.h"

namespace cyclus {

/// ExchangeCase, a class that provides a virtual interface for constructing
/// and testing a variety of ExchangeGraph/ExchangeSolver test instances
class ExchangeCase {
public:
  /// constructs an instance of an exchange graph for the test case
  virtual void Construct(ExchangeGraph* g) = 0;

  virtual void Test(std::string solver_type, ExchangeGraph* g) = 0;
};

/// Case0, an empty graph
/// no matches should exist
class Case0: public ExchangeCase {
  virtual void Construct(ExchangeGraph* g) {}
  
  virtual void Test(std::string solver_type, ExchangeGraph* g) {
    EXPECT_TRUE(g->matches().empty());
  }
};

/// Case1a, a single request group
/// no matches should exist
class Case1a: public ExchangeCase {
  virtual void Construct(ExchangeGraph* g) {
    RequestGroup::Ptr group = RequestGroup::Ptr(new RequestGroup());
    ExchangeNode::Ptr n = ExchangeNode::Ptr(new ExchangeNode());
    group->AddExchangeNode(n);
    g->AddRequestGroup(group);
  }
  
  virtual void Test(std::string solver_type, ExchangeGraph* g) {
    EXPECT_TRUE(g->matches().empty()) << solver_type; 
  }
};

/// Case1b, a single supply group
/// no matches should exist
class Case1b: public ExchangeCase {
  virtual void Construct(ExchangeGraph* g) {
    ExchangeNodeGroup::Ptr group = ExchangeNodeGroup::Ptr(new ExchangeNodeGroup());
    ExchangeNode::Ptr n = ExchangeNode::Ptr(new ExchangeNode());
    group->AddExchangeNode(n);
    g->AddSupplyGroup(group);
  }
  
  virtual void Test(std::string solver_type, ExchangeGraph* g) {
    EXPECT_TRUE(g->matches().empty());
  }
};

} // namespace cyclus

#endif // ifndef CYCLUS_TESTS_EXCHANGE_TEST_CASES_H_
