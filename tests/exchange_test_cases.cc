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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case2::Construct(ExchangeGraph* g) {
  // using cyclus::ExchangeGraph;
  // using cyclus::ExchangeNode;
  // using cyclus::ExchangeNodeGroup;
  // using cyclus::RequestGroup;
  // using cyclus::Arc;
  // using cyclus::Match;
  
  ExchangeNode::Ptr u(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());
  Arc a(u, v);

  u->unit_capacities[a].push_back(unit_cap_req);
  v->unit_capacities[a].push_back(unit_cap_sup);
  
  RequestGroup::Ptr request(new RequestGroup(qty));
  request->AddCapacity(qty);
  request->AddExchangeNode(u);  
  g->AddRequestGroup(request);

  ExchangeNodeGroup::Ptr supply(new ExchangeNodeGroup());
  supply->AddCapacity(capacity);
  supply->AddExchangeNode(v);  
  g->AddSupplyGroup(supply);

  g->AddArc(a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case2::Test(std::string solver_type, ExchangeGraph* g) {
  if (solver_type == "greedy") {
    ASSERT_TRUE(g->arcs().size() > 0);
    Match exp = Match(g->arcs().at(0), flow);
    ASSERT_TRUE(g->matches().size() > 0);
    EXPECT_EQ(exp, g->matches().at(0));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case2a::Construct(ExchangeGraph* g) {
  // set 2a members
  qty = 5;
  unit_cap_req = 1;
  capacity = 10;
  unit_cap_sup = 1;
  flow = qty;

  Case2::Construct(g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case2b::Construct(ExchangeGraph* g) {
  // set 2b members
  qty = 10;
  unit_cap_req = 1;
  capacity = 10;
  unit_cap_sup = 2;
  flow = capacity / unit_cap_sup;

  Case2::Construct(g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case2c::Construct(ExchangeGraph* g) {
  // set 2c members
  qty = 10;
  unit_cap_req = 1;
  capacity = 5;
  unit_cap_sup = 1;
  flow = capacity;

  Case2::Construct(g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case2d::Construct(ExchangeGraph* g) {
  // set 2d members
  qty = 10;
  unit_cap_req = 1;
  capacity = 5;
  unit_cap_sup = 0.3;
  flow = qty;

  Case2::Construct(g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case2e::Construct(ExchangeGraph* g) {
  // set 2e members
  qty = 10;
  unit_cap_req = 1;
  capacity = 10;
  unit_cap_sup = 0.3;
  flow = qty;

  Case2::Construct(g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case2f::Construct(ExchangeGraph* g) {
  // set 2f members
  qty = 10;
  unit_cap_req = 1;
  capacity = 10;
  unit_cap_sup = 2;
  flow = capacity / unit_cap_sup;

  Case2::Construct(g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case2g::Construct(ExchangeGraph* g) {
  // set 2g members
  qty = 10;
  unit_cap_req = 0.9;
  capacity = 10;
  unit_cap_sup = 1;
  flow = capacity;

  Case2::Construct(g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case2h::Construct(ExchangeGraph* g) {
  // set 2h members
  qty = 10;
  unit_cap_req = 2;
  capacity = 10;
  unit_cap_sup = 1;
  flow = capacity / unit_cap_req;

  Case2::Construct(g);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case7::Construct(ExchangeGraph* g) {
  qty = 5;
  N = 10;
  flow = qty / N;
  
  // a single request for qty of a resource
  ExchangeNode::Ptr u(new ExchangeNode());
  RequestGroup::Ptr req(new RequestGroup(qty));
  req->AddCapacity(qty);
  req->AddExchangeNode(u);
  g->AddRequestGroup(req);

  // a node group with N bids for q/N of a resource
  ExchangeNodeGroup::Ptr sup(new ExchangeNodeGroup());
  sup->AddCapacity(qty);  
  for (int i = 0; i < N; i++) {
    ExchangeNode::Ptr v(new ExchangeNode(qty / N)); 
    sup->AddExchangeNode(v);  
    Arc a(u, v);
    u->unit_capacities[a].push_back(1);
    v->unit_capacities[a].push_back(1);
    g->AddArc(a);
  }
  g->AddSupplyGroup(sup);    
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Case7::Test(std::string solver_type, ExchangeGraph* g) {
  if (solver_type == "greedy") {
    ASSERT_EQ(g->arcs().size(), N);
    ASSERT_EQ(g->matches().size(), N);
    for (int i = 0; i < N; i++) {
      Match exp = Match(g->arcs().at(i), flow);
      EXPECT_EQ(exp, g->matches().at(i));
    }
  }
}

} // namespace cyclus
