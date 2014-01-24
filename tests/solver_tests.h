#ifndef CYCLUS_TESTS_SOLVER_TESTS_H_
#define CYCLUS_TESTS_SOLVER_TESTS_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "exchange_graph.h"
#include "exchange_solver.h"
#include "greedy_solver.h"

namespace cyclus {

class SolverCase {
public:
  /// constructs an instance of an exchange graph for the test case
  virtual void Construct(ExchangeGraph* g) = 0;

  virtual void Test(std::string solver_type, ExchangeGraph* g) = 0;
};

class Case0: public SolverCase {
  virtual void Construct(ExchangeGraph* g) {}
  
  virtual void Test(std::string solver_type, ExchangeGraph* g) {
    EXPECT_TRUE(g->matches().empty());
  }
};

class Case1: public SolverCase {
  virtual void Construct(ExchangeGraph* g) = 0;
  
  virtual void Test(std::string solver_type, ExchangeGraph* g) {
    EXPECT_TRUE(g->matches().empty());
  }
};

class Case1a: public Case1 {
  virtual void Construct(ExchangeGraph* g) {
    RequestGroup::Ptr group = RequestGroup::Ptr(new RequestGroup());
    ExchangeNode::Ptr n = ExchangeNode::Ptr(new ExchangeNode());
    group->AddExchangeNode(n);
    g->AddRequestGroup(group);
  }
};

class Case1b: public Case1 {
  virtual void Construct(ExchangeGraph* g) {
    ExchangeNodeGroup::Ptr group = ExchangeNodeGroup::Ptr(new ExchangeNodeGroup());
    ExchangeNode::Ptr n = ExchangeNode::Ptr(new ExchangeNode());
    group->AddExchangeNode(n);
    g->AddSupplyGroup(group);
  }
};

class SolverFactory {
 public:
  SolverFactory() {
    solver_types_.push_back("greedy");
  }

  boost::shared_ptr<ExchangeSolver> Solver(std::string solver_type,
                                           ExchangeGraph* g) {
    boost::shared_ptr<ExchangeSolver> ret;
    if (solver_type == "greedy") {
      ret = boost::shared_ptr<ExchangeSolver>(new GreedySolver(g));
    }
    return ret;
  }
  
  std::vector<std::string> solver_types() { return solver_types_; }

 private:
  std::vector<std::string> solver_types_;
};

} // namespace cyclus

#endif // ifndef CYCLUS_TESTS_SOLVER_TESTS_H_
