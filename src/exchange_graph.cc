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
void NodeSet::UpdateCapacities(Node::Ptr node, double qty) {
  using std::vector;
  using cyclus::DoubleNeg;
  using cyclus::ValueError;
  
  const vector<double>& units = node->unit_capacities;
  for (int i = 0; i < capacities.size(); i++) {
    double val = capacities[i] - qty * units[i];
    if (DoubleNeg(val)) throw ValueError("Capacities can not be reduced below 0.");
    capacities[i] = val;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RequestSet::RequestSet(double qty) : qty(qty) { }

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

  std::vector<double> caps;
  const std::vector<double>& unit_caps = n.unit_capacities;
  const std::vector<double>& set_caps = n.set->capacities;
  for (int i = 0; i < unit_caps.size(); i++) {
    caps.push_back(set_caps[i] / unit_caps[i]);
  }

  return *std::min_element(caps.begin(), caps.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Capacity(const Node::Ptr& pn) {
  return Capacity(*pn.get());
}

} // namespace cyclus
