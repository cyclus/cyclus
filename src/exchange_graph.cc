#include <algorithm>

#include "cyc_limits.h"
#include "error.h"

#include "exchange_graph.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Node::Node() : set(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void NodeSet::AddNode(Node::Ptr node) {
  node->set = this;
  nodes.push_back(node);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RequestSet::RequestSet(double qty) : qty(qty) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Arc::Arc(Node::Ptr unode, Node::Ptr vnode) : unode(unode), vnode(vnode) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Capacity(const Arc& a) {
  double ucap = Capacity(a.unode);
  double vcap = Capacity(a.vnode);
  return std::min(ucap, vcap);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Capacity(const Node& n) {
  if (n.set == NULL) {
    throw cyclus::StateError("An notion of node capacity requires a nodeset.");
  }

  if (n.unit_capacities.size() == 0) {
    return std::numeric_limits<double>::max();
  }

  const std::vector<double>& unit_caps = n.unit_capacities;
  const std::vector<double>& set_caps = n.set->capacities;
  std::vector<double> caps;
  for (int i = 0; i < unit_caps.size(); i++) {
    caps.push_back(set_caps[i] / unit_caps[i]);
  }

  return *std::min_element(caps.begin(), caps.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Capacity(Node::Ptr pn) {
  return Capacity(*pn.get());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UpdateCapacity(const Node& n, double qty) {
  using std::vector;
  using cyclus::DoubleNeg;
  using cyclus::ValueError;

  if (n.set == NULL) {
    throw cyclus::StateError("An notion of node capacity requires a nodeset.");
  }
  
  const vector<double>& unit_caps = n.unit_capacities;
  vector<double>& caps = n.set->capacities;
  for (int i = 0; i < caps.size(); i++) {
    double val = caps[i] - qty * unit_caps[i];
    if (DoubleNeg(val)) throw ValueError("Capacities can not be reduced below 0.");
    caps[i] = val;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UpdateCapacity(Node::Ptr pn, double qty) {
  return UpdateCapacity(*pn.get(), qty);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddRequestSet(RequestSet::Ptr prs) {
  request_sets.push_back(prs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddSupplySet(NodeSet::Ptr pss) {
  supply_sets.push_back(pss);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddArc(Arc::Ptr pa) {
  arcs_.push_back(pa);
  node_arc_map[pa->unode].push_back(pa);
  node_arc_map[pa->vnode].push_back(pa);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddMatch(Arc::Ptr pa, double qty) {
  UpdateCapacity(pa->unode, qty);
  UpdateCapacity(pa->vnode, qty);
  matches.push_back(std::make_pair(pa, qty));
}

} // namespace cyclus
