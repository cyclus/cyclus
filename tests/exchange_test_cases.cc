#include <gtest/gtest.h>

#include "exchange_test_cases.h"

#include "exchange_graph.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case0::Construct(ExchangeGraph* g) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case0::Test(std::string solver_type, ExchangeGraph* g) {
  EXPECT_TRUE(g->matches().empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case1a::Construct(ExchangeGraph* g) {
  RequestGroup::Ptr group = RequestGroup::Ptr(new RequestGroup());
  ExchangeNode::Ptr n = ExchangeNode::Ptr(new ExchangeNode());
  group->AddExchangeNode(n);
  g->AddRequestGroup(group);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case1a::Test(std::string solver_type, ExchangeGraph* g) {
  EXPECT_TRUE(g->matches().empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case1b::Construct(ExchangeGraph* g) {
  ExchangeNodeGroup::Ptr group = ExchangeNodeGroup::Ptr(new ExchangeNodeGroup());
  ExchangeNode::Ptr n = ExchangeNode::Ptr(new ExchangeNode());
  group->AddExchangeNode(n);
  g->AddSupplyGroup(group);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case1b::Test(std::string solver_type, ExchangeGraph* g) {
  EXPECT_TRUE(g->matches().empty());
}

} // namespace cyclus
