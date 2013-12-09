#ifndef CYCLUS_TESTS_GREEDY_SOLVER_TESTS_H_
#define CYCLUS_TESTS_GREEDY_SOLVER_TESTS_H_

#include "exchange_graph.h"
#include "greedy_solver.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp1a() {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::RequestGroup;

  ExchangeGraph g;
  RequestGroup::Ptr group = RequestGroup::Ptr(new RequestGroup());
  ExchangeNode::Ptr n = ExchangeNode::Ptr(new ExchangeNode());
  group->AddExchangeNode(n);
  g.AddRequestGroup(group);
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp1b() {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeGroup;
  
  ExchangeGraph g;
  ExchangeNodeGroup::Ptr group = ExchangeNodeGroup::Ptr(new ExchangeNodeGroup());
  ExchangeNode::Ptr n = ExchangeNode::Ptr(new ExchangeNode());
  group->AddExchangeNode(n);
  g.AddSupplyGroup(group);
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp2(double qty, double unit_cap_req,
                             double capacity, double unit_cap_sup) {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeGroup;
  using cyclus::RequestGroup;
  using cyclus::Arc;
  using cyclus::Match;
  
  ExchangeGraph g;
  
  ExchangeNode::Ptr u(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());
  Arc a(u, v);

  u->unit_capacities[a].push_back(unit_cap_req);
  v->unit_capacities[a].push_back(unit_cap_sup);
  
  RequestGroup::Ptr request(new RequestGroup(qty));
  request->AddCapacity(qty);
  request->AddExchangeNode(u);  
  g.AddRequestGroup(request);

  ExchangeNodeGroup::Ptr supply(new ExchangeNodeGroup());
  supply->AddCapacity(capacity);
  supply->AddExchangeNode(v);  
  g.AddSupplyGroup(supply);

  g.AddArc(a);
  
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp3(double qty, double cap1, double cap2,
                             double pref1 = 0.0, double pref2 = 0.0) {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeGroup;
  using cyclus::RequestGroup;
  using cyclus::Arc;
  using cyclus::Match;
  
  ExchangeGraph g;
  
  ExchangeNode::Ptr u(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());
  ExchangeNode::Ptr w(new ExchangeNode());
  Arc a1(u, v);
  Arc a2(u, w);

  u->unit_capacities[a1].push_back(1);
  v->unit_capacities[a1].push_back(1);
  u->unit_capacities[a2].push_back(1);
  w->unit_capacities[a2].push_back(1);
  u->prefs[a1] = pref1;
  u->prefs[a2] = pref2;

  RequestGroup::Ptr request(new RequestGroup(qty));
  request->AddCapacity(qty);
  request->AddExchangeNode(u);  
  g.AddRequestGroup(request);
  
  ExchangeNodeGroup::Ptr supply1(new ExchangeNodeGroup());
  supply1->AddCapacity(cap1);
  supply1->AddExchangeNode(v);  
  g.AddSupplyGroup(supply1);
  
  ExchangeNodeGroup::Ptr supply2(new ExchangeNodeGroup());
  supply2->AddCapacity(cap2);
  supply2->AddExchangeNode(w);  
  g.AddSupplyGroup(supply2);

  g.AddArc(a1);
  g.AddArc(a2);
  
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp4(double qty1, double qty2, double cap) {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeGroup;
  using cyclus::RequestGroup;
  using cyclus::Arc;
  using cyclus::Match;
  
  ExchangeGraph g;
  
  ExchangeNode::Ptr u(new ExchangeNode());
  ExchangeNode::Ptr v1(new ExchangeNode());
  ExchangeNode::Ptr w(new ExchangeNode());
  ExchangeNode::Ptr v2(new ExchangeNode());
  Arc a1(u, v1);
  Arc a2(w, v2);

  u->unit_capacities[a1].push_back(1);
  v1->unit_capacities[a1].push_back(1);
  w->unit_capacities[a2].push_back(1);
  v2->unit_capacities[a2].push_back(1);

  RequestGroup::Ptr req1(new RequestGroup(qty1));
  req1->AddCapacity(qty1);
  req1->AddExchangeNode(u);  
  g.AddRequestGroup(req1);
  
  RequestGroup::Ptr req2(new RequestGroup(qty2));
  req2->AddCapacity(qty2);
  req2->AddExchangeNode(w);  
  g.AddRequestGroup(req2);
  
  ExchangeNodeGroup::Ptr supply(new ExchangeNodeGroup());
  supply->AddCapacity(cap);
  supply->AddExchangeNode(v1);  
  supply->AddExchangeNode(v2);  
  g.AddSupplyGroup(supply);
  
  g.AddArc(a1);
  g.AddArc(a2);
  
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp5(double qty, double cap1, double cap2) {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeGroup;
  using cyclus::RequestGroup;
  using cyclus::Arc;
  using cyclus::Match;
  
  ExchangeGraph g;
  
  ExchangeNode::Ptr u1(new ExchangeNode());
  ExchangeNode::Ptr u2(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());
  ExchangeNode::Ptr w(new ExchangeNode());
  Arc a1(u1, v);
  Arc a2(u2, w);

  u1->unit_capacities[a1].push_back(1);
  v->unit_capacities[a1].push_back(1);
  u2->unit_capacities[a2].push_back(1);
  w->unit_capacities[a2].push_back(1);

  RequestGroup::Ptr request(new RequestGroup(qty));
  request->AddCapacity(qty);
  request->AddExchangeNode(u1);
  request->AddExchangeNode(u2);  
  g.AddRequestGroup(request);
  
  ExchangeNodeGroup::Ptr supply1(new ExchangeNodeGroup());
  supply1->AddCapacity(cap1);
  supply1->AddExchangeNode(v);  
  g.AddSupplyGroup(supply1);
  
  ExchangeNodeGroup::Ptr supply2(new ExchangeNodeGroup());
  supply2->AddCapacity(cap2);
  supply2->AddExchangeNode(w);  
  g.AddSupplyGroup(supply2);

  g.AddArc(a1);
  g.AddArc(a2);
  
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp6(double qty1, double qty2,
                             double cap1, double cap2) {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeGroup;
  using cyclus::RequestGroup;
  using cyclus::Arc;
  using cyclus::Match;
  
  ExchangeGraph g;
  
  ExchangeNode::Ptr u1_1(new ExchangeNode());
  ExchangeNode::Ptr u1_2(new ExchangeNode());
  ExchangeNode::Ptr u2_1(new ExchangeNode());
  ExchangeNode::Ptr u2_2(new ExchangeNode());
  ExchangeNode::Ptr v1_1(new ExchangeNode());
  ExchangeNode::Ptr v1_2(new ExchangeNode());
  ExchangeNode::Ptr v2_1(new ExchangeNode());
  ExchangeNode::Ptr v2_2(new ExchangeNode());
  Arc a1(u1_1, v1_1);
  Arc a2(u1_2, v2_1);
  Arc a3(u2_1, v1_2);
  Arc a4(u2_2, v2_2);

  u1_1->unit_capacities[a1].push_back(1);
  u1_2->unit_capacities[a2].push_back(1);
  u2_1->unit_capacities[a3].push_back(1);
  u2_2->unit_capacities[a4].push_back(1);
  v1_1->unit_capacities[a1].push_back(1);
  v1_2->unit_capacities[a3].push_back(1);
  v2_1->unit_capacities[a2].push_back(1);
  v2_2->unit_capacities[a4].push_back(1);

  RequestGroup::Ptr req1(new RequestGroup(qty1));
  req1->AddCapacity(qty1);
  req1->AddExchangeNode(u1_1);
  req1->AddExchangeNode(u1_2);  
  g.AddRequestGroup(req1);
  
  RequestGroup::Ptr req2(new RequestGroup(qty2));
  req2->AddCapacity(qty2);
  req2->AddExchangeNode(u2_1);
  req2->AddExchangeNode(u2_2);  
  g.AddRequestGroup(req2);
  
  ExchangeNodeGroup::Ptr sup1(new ExchangeNodeGroup());
  sup1->AddCapacity(cap1);
  sup1->AddExchangeNode(v1_1);  
  sup1->AddExchangeNode(v1_2);  
  g.AddSupplyGroup(sup1);
  
  ExchangeNodeGroup::Ptr sup2(new ExchangeNodeGroup());
  sup2->AddCapacity(cap2);
  sup2->AddExchangeNode(v2_1);
  sup2->AddExchangeNode(v2_2);
  g.AddSupplyGroup(sup2);
  
  g.AddArc(a1);
  g.AddArc(a2);
  g.AddArc(a3);
  g.AddArc(a4);
  
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp7(double qty, int N) {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeGroup;
  using cyclus::RequestGroup;
  using cyclus::Arc;
  using cyclus::Match;
  
  ExchangeGraph g;

  // a single request for qty of a resource
  ExchangeNode::Ptr u(new ExchangeNode());
  RequestGroup::Ptr req(new RequestGroup(qty));
  req->AddCapacity(qty);
  req->AddExchangeNode(u);
  g.AddRequestGroup(req);

  // a node group with N bids for q/N of a resource
  ExchangeNodeGroup::Ptr sup(new ExchangeNodeGroup());
  sup->AddCapacity(qty);  
  for (int i = 0; i < N; i++) {
    ExchangeNode::Ptr v(new ExchangeNode(qty / N)); 
    sup->AddExchangeNode(v);  
    Arc a(u, v);
    u->unit_capacities[a].push_back(1);
    v->unit_capacities[a].push_back(1);
    g.AddArc(a);
  }
  g.AddSupplyGroup(sup);    
  
  return g;
};

#endif // ifndef CYCLUS_TESTS_GREEDY_SOLVER_TESTS_H_
