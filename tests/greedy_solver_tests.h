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
cyclus::ExchangeGraph SetUp2(double qty, double capacity) {
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
  v->unit_capacities[a.get()].push_back(1);
  
  NodeSet::Ptr supply(new NodeSet());
  supply->capacities.push_back(capacity);
  supply->AddNode(u);  
  g.AddSupplySet(supply);

  RequestSet::Ptr request(new RequestSet(qty));
  request->capacities.push_back(qty*5); // some large number
  request->AddNode(v);  
  g.AddRequestSet(request);

  g.AddArc(a);
  
  return g;
};

#endif // ifndef CYCLUS_TESTS_GREEDY_SOLVER_TESTS_H_
