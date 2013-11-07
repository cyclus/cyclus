
#include "cyc_limits.h"
#include "error.h"

#include "exchange_graph.h"

namespace cyclus {

void NodeSet::AddNode(Node::Ptr node) {
  node->set = this;
  nodes.push_back(node);
}

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

} // namespace cyclus
