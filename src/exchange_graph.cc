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
double Arc::capacity() {
  if (unode->set == NULL || vnode->set == NULL) {
    throw cyclus::StateError("An arc's nodes must be part of node sets.");
  }
  
  if (unode->unit_capacities.size() == 0 && vnode->unit_capacities.size() == 0) {
    return std::numeric_limits<double>::max();
  }

  std::vector<double> all_caps;
  const std::vector<double>& uunit_caps = unode->unit_capacities;
  const std::vector<double>& ucaps = unode->set->capacities;
  for (int i = 0; i < ucaps.size(); i++) {
    all_caps.push_back(ucaps[i] / uunit_caps[i]);
  }
  
  const std::vector<double>& vunit_caps = vnode->unit_capacities;
  const std::vector<double>& vcaps = vnode->set->capacities;
  for (int i = 0; i < vcaps.size(); i++) {
    all_caps.push_back(vcaps[i] / vunit_caps[i]);
  }
  
  return *std::min_element(all_caps.begin(), all_caps.end());
}

} // namespace cyclus
