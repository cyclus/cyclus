#ifndef CYCLUS_TESTS_GREEDY_SOLVER_TESTS_H_
#define CYCLUS_TESTS_GREEDY_SOLVER_TESTS_H_

#include "exchange_graph.h"
#include "greedy_solver.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp1a() {
  using cyclus::ExchangeGraph;
  using cyclus::Node;
  using cyclus::RequestSet;

  ExchangeGraph g;
  RequestSet::Ptr set = RequestSet::Ptr(new RequestSet());
  Node::Ptr n = Node::Ptr(new Node());
  set->AddNode(n);
  g.AddRequestSet(set);
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp1b() {
  using cyclus::ExchangeGraph;
  using cyclus::Node;
  using cyclus::NodeSet;
  
  ExchangeGraph g;
  NodeSet::Ptr set = NodeSet::Ptr(new NodeSet());
  Node::Ptr n = Node::Ptr(new Node());
  set->AddNode(n);
  g.AddSupplySet(set);
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp2(double qty, double capacity, double unit_cap) {
  using cyclus::ExchangeGraph;
  using cyclus::Node;
  using cyclus::NodeSet;
  using cyclus::RequestSet;
  using cyclus::Arc;
  using cyclus::Match;
  
  ExchangeGraph g;
  
  Node::Ptr u(new Node());
  Node::Ptr v(new Node());
  Arc::Ptr a(new Arc(u, v));

  u->unit_capacities[a.get()].push_back(1);
  v->unit_capacities[a.get()].push_back(unit_cap);
  
  RequestSet::Ptr request(new RequestSet(qty));
  request->capacities.push_back(qty*5); // some large number
  request->AddNode(u);  
  g.AddRequestSet(request);

  NodeSet::Ptr supply(new NodeSet());
  supply->capacities.push_back(capacity);
  supply->AddNode(v);  
  g.AddSupplySet(supply);

  g.AddArc(a);
  
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp3(double qty, double cap1, double cap2) {
  using cyclus::ExchangeGraph;
  using cyclus::Node;
  using cyclus::NodeSet;
  using cyclus::RequestSet;
  using cyclus::Arc;
  using cyclus::Match;
  
  ExchangeGraph g;
  
  Node::Ptr u(new Node());
  Node::Ptr v(new Node());
  Node::Ptr w(new Node());
  Arc::Ptr a1(new Arc(u, v));
  Arc::Ptr a2(new Arc(u, w));

  u->unit_capacities[a1.get()].push_back(1);
  v->unit_capacities[a1.get()].push_back(1);
  u->unit_capacities[a2.get()].push_back(1);
  w->unit_capacities[a2.get()].push_back(1);

  RequestSet::Ptr request(new RequestSet(qty));
  request->capacities.push_back(qty*5); // some large number
  request->AddNode(u);  
  g.AddRequestSet(request);
  
  NodeSet::Ptr supply1(new NodeSet());
  supply1->capacities.push_back(cap1);
  supply1->AddNode(v);  
  g.AddSupplySet(supply1);
  
  NodeSet::Ptr supply2(new NodeSet());
  supply2->capacities.push_back(cap2);
  supply2->AddNode(w);  
  g.AddSupplySet(supply2);

  g.AddArc(a1);
  g.AddArc(a2);
  
  return g;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp4(double qty1, double qty2, double cap) {
  using cyclus::ExchangeGraph;
  using cyclus::Node;
  using cyclus::NodeSet;
  using cyclus::RequestSet;
  using cyclus::Arc;
  using cyclus::Match;
  
  ExchangeGraph g;
  
  Node::Ptr u(new Node());
  Node::Ptr v1(new Node());
  Node::Ptr w(new Node());
  Node::Ptr v2(new Node());
  Arc::Ptr a1(new Arc(u, v1));
  Arc::Ptr a2(new Arc(w, v2));

  u->unit_capacities[a1.get()].push_back(1);
  v1->unit_capacities[a1.get()].push_back(1);
  w->unit_capacities[a2.get()].push_back(1);
  v2->unit_capacities[a2.get()].push_back(1);

  RequestSet::Ptr req1(new RequestSet(qty1));
  req1->capacities.push_back(qty1*5); // some large number
  req1->AddNode(u);  
  g.AddRequestSet(req1);
  
  RequestSet::Ptr req2(new RequestSet(qty2));
  req2->capacities.push_back(qty2*5); // some large number
  req2->AddNode(w);  
  g.AddRequestSet(req2);
  
  NodeSet::Ptr supply(new NodeSet());
  supply->capacities.push_back(cap);
  supply->AddNode(v1);  
  supply->AddNode(v2);  
  g.AddSupplySet(supply);
  
  g.AddArc(a1);
  g.AddArc(a2);
  
  return g;
};

#endif // ifndef CYCLUS_TESTS_GREEDY_SOLVER_TESTS_H_
