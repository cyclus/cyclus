#ifndef CYCLUS_TESTS_GREEDY_SOLVER_TESTS_H_
#define CYCLUS_TESTS_GREEDY_SOLVER_TESTS_H_

#include "exchange_graph.h"
#include "greedy_solver.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp1a() {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::RequestSet;

  ExchangeGraph g;
  RequestSet::Ptr set = RequestSet::Ptr(new RequestSet());
  ExchangeNode::Ptr n = ExchangeNode::Ptr(new ExchangeNode());
  set->AddExchangeNode(n);
  g.AddRequestSet(set);
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp1b() {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeSet;
  
  ExchangeGraph g;
  ExchangeNodeSet::Ptr set = ExchangeNodeSet::Ptr(new ExchangeNodeSet());
  ExchangeNode::Ptr n = ExchangeNode::Ptr(new ExchangeNode());
  set->AddExchangeNode(n);
  g.AddSupplySet(set);
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp2(double qty, double unit_cap_req,
                             double capacity, double unit_cap_sup) {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeSet;
  using cyclus::RequestSet;
  using cyclus::Arc;
  using cyclus::Match;
  
  ExchangeGraph g;
  
  ExchangeNode::Ptr u(new ExchangeNode());
  ExchangeNode::Ptr v(new ExchangeNode());
  Arc a(u, v);

  u->unit_capacities[a].push_back(unit_cap_req);
  v->unit_capacities[a].push_back(unit_cap_sup);
  
  RequestSet::Ptr request(new RequestSet(qty));
  request->capacities.push_back(qty);
  request->AddExchangeNode(u);  
  g.AddRequestSet(request);

  ExchangeNodeSet::Ptr supply(new ExchangeNodeSet());
  supply->capacities.push_back(capacity);
  supply->AddExchangeNode(v);  
  g.AddSupplySet(supply);

  g.AddArc(a);
  
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp3(double qty, double cap1, double cap2) {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeSet;
  using cyclus::RequestSet;
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

  RequestSet::Ptr request(new RequestSet(qty));
  request->capacities.push_back(qty);
  request->AddExchangeNode(u);  
  g.AddRequestSet(request);
  
  ExchangeNodeSet::Ptr supply1(new ExchangeNodeSet());
  supply1->capacities.push_back(cap1);
  supply1->AddExchangeNode(v);  
  g.AddSupplySet(supply1);
  
  ExchangeNodeSet::Ptr supply2(new ExchangeNodeSet());
  supply2->capacities.push_back(cap2);
  supply2->AddExchangeNode(w);  
  g.AddSupplySet(supply2);

  g.AddArc(a1);
  g.AddArc(a2);
  
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp4(double qty1, double qty2, double cap) {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeSet;
  using cyclus::RequestSet;
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

  RequestSet::Ptr req1(new RequestSet(qty1));
  req1->capacities.push_back(qty1);
  req1->AddExchangeNode(u);  
  g.AddRequestSet(req1);
  
  RequestSet::Ptr req2(new RequestSet(qty2));
  req2->capacities.push_back(qty2);
  req2->AddExchangeNode(w);  
  g.AddRequestSet(req2);
  
  ExchangeNodeSet::Ptr supply(new ExchangeNodeSet());
  supply->capacities.push_back(cap);
  supply->AddExchangeNode(v1);  
  supply->AddExchangeNode(v2);  
  g.AddSupplySet(supply);
  
  g.AddArc(a1);
  g.AddArc(a2);
  
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp5(double qty, double cap1, double cap2) {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeSet;
  using cyclus::RequestSet;
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

  RequestSet::Ptr request(new RequestSet(qty));
  request->capacities.push_back(qty);
  request->AddExchangeNode(u1);
  request->AddExchangeNode(u2);  
  g.AddRequestSet(request);
  
  ExchangeNodeSet::Ptr supply1(new ExchangeNodeSet());
  supply1->capacities.push_back(cap1);
  supply1->AddExchangeNode(v);  
  g.AddSupplySet(supply1);
  
  ExchangeNodeSet::Ptr supply2(new ExchangeNodeSet());
  supply2->capacities.push_back(cap2);
  supply2->AddExchangeNode(w);  
  g.AddSupplySet(supply2);

  g.AddArc(a1);
  g.AddArc(a2);
  
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp6(double qty1, double qty2,
                             double cap1, double cap2) {
  using cyclus::ExchangeGraph;
  using cyclus::ExchangeNode;
  using cyclus::ExchangeNodeSet;
  using cyclus::RequestSet;
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

  RequestSet::Ptr req1(new RequestSet(qty1));
  req1->capacities.push_back(qty1);
  req1->AddExchangeNode(u1_1);
  req1->AddExchangeNode(u1_2);  
  g.AddRequestSet(req1);
  
  RequestSet::Ptr req2(new RequestSet(qty2));
  req2->capacities.push_back(qty2);
  req2->AddExchangeNode(u2_1);
  req2->AddExchangeNode(u2_2);  
  g.AddRequestSet(req2);
  
  ExchangeNodeSet::Ptr sup1(new ExchangeNodeSet());
  sup1->capacities.push_back(cap1);
  sup1->AddExchangeNode(v1_1);  
  sup1->AddExchangeNode(v1_2);  
  g.AddSupplySet(sup1);
  
  ExchangeNodeSet::Ptr sup2(new ExchangeNodeSet());
  sup2->capacities.push_back(cap2);
  sup2->AddExchangeNode(v2_1);
  sup2->AddExchangeNode(v2_2);
  g.AddSupplySet(sup2);
  
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
  using cyclus::ExchangeNodeSet;
  using cyclus::RequestSet;
  using cyclus::Arc;
  using cyclus::Match;
  
  ExchangeGraph g;

  // a single request for qty of a resource
  ExchangeNode::Ptr u(new ExchangeNode());
  RequestSet::Ptr req(new RequestSet(qty));
  req->capacities.push_back(qty);
  req->AddExchangeNode(u);
  g.AddRequestSet(req);

  // a node set with N bids for q/N of a resource
  ExchangeNodeSet::Ptr sup(new ExchangeNodeSet());
  sup->capacities.push_back(qty);  
  for (int i = 0; i < N; i++) {
    ExchangeNode::Ptr v(new ExchangeNode(qty / N)); 
    sup->AddExchangeNode(v);  
    Arc a(u, v);
    u->unit_capacities[a].push_back(1);
    v->unit_capacities[a].push_back(1);
    g.AddArc(a);
  }
  g.AddSupplySet(sup);    
  
  return g;
};

#endif // ifndef CYCLUS_TESTS_GREEDY_SOLVER_TESTS_H_
