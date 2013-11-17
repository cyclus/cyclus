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
// make a node with a single unit capacity of 1.0
cyclus::Node::Ptr UnitNode() {
  using cyclus::Node;
  Node::Ptr n = Node::Ptr(new Node());
  n->unit_capacities.push_back(1);
  return n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cyclus::ExchangeGraph SetUp2(double qty, double capacity) {
  using cyclus::ExchangeGraph;
  using cyclus::Node;
  using cyclus::NodeSet;
  using cyclus::RequestSet;
  using cyclus::Arc;
  using cyclus::Match;
  
  ExchangeGraph g;
  
  NodeSet::Ptr supply = NodeSet::Ptr(new NodeSet());
  Node::Ptr u = UnitNode();
  supply->capacities.push_back(capacity);
  supply->AddNode(u);  
  g.AddSupplySet(supply);

  RequestSet::Ptr request = RequestSet::Ptr(new RequestSet(qty));
  Node::Ptr v = UnitNode();
  request->capacities.push_back(qty*5); // some large number
  request->AddNode(v);  
  g.AddRequestSet(request);

  Arc::Ptr a = Arc::Ptr(new Arc(u, v));
  g.AddArc(a);
  
  return g;
};

#endif // ifndef CYCLUS_TESTS_GREEDY_SOLVER_TESTS_H_
