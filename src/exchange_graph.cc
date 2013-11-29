#include <algorithm>

#include "cyc_limits.h"
#include "error.h"

#include "exchange_graph.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ExchangeNode::ExchangeNode(double max_qty) : max_qty(max_qty), qty(0), set(NULL) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool operator==(const ExchangeNode& lhs, const ExchangeNode& rhs) {
  return (lhs.unit_capacities == rhs.unit_capacities &&
          lhs.max_qty == rhs.max_qty &&
          lhs.set == rhs.set);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeNodeSet::AddExchangeNode(ExchangeNode::Ptr node) {
  node->set = this;
  nodes.push_back(node);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RequestSet::RequestSet(double qty) : qty(qty) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Capacity(const Arc& a) {
  double ucap = Capacity(a.first, a);
  double vcap = Capacity(a.second, a);
  return std::min(ucap, vcap);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Capacity(ExchangeNode& n, const Arc& a) {
  if (n.set == NULL) {
    throw cyclus::StateError("An notion of node capacity requires a nodeset.");
  }

  if (n.unit_capacities[a].size() == 0) {
    return std::numeric_limits<double>::max();
  }

  std::vector<double>& unit_caps = n.unit_capacities[a];
  const std::vector<double>& set_caps = n.set->capacities;
  std::vector<double> caps;
  for (int i = 0; i < unit_caps.size(); i++) {
    caps.push_back(set_caps[i] / unit_caps[i]);
  }

  return std::min(*std::min_element(caps.begin(), caps.end()), n.max_qty - n.qty);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Capacity(ExchangeNode::Ptr pn, const Arc& a) {
  return Capacity(*pn.get(), a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UpdateCapacity(ExchangeNode& n, const Arc& a, double qty) {
  using std::vector;
  using cyclus::IsNegative;
  using cyclus::ValueError;

  if (n.set == NULL) {
    throw cyclus::StateError("An notion of node capacity requires a nodeset.");
  }
  
  vector<double>& unit_caps = n.unit_capacities[a];
  vector<double>& caps = n.set->capacities;
  for (int i = 0; i < caps.size(); i++) {
    double val = caps[i] - qty * unit_caps[i];
    if (IsNegative(val)) throw ValueError("Capacities can not be reduced below 0.");
    caps[i] = val;
  }

  double val = n.max_qty - qty;
  if (IsNegative(val)) {
    throw ValueError("ExchangeNode quantities can not be reduced below 0.");
  }
  n.qty += qty;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UpdateCapacity(ExchangeNode::Ptr pn, const Arc& a, double qty) {
  return UpdateCapacity(*pn.get(), a, qty);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddRequestSet(RequestSet::Ptr prs) {
  request_sets.push_back(prs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddSupplySet(ExchangeNodeSet::Ptr pss) {
  supply_sets.push_back(pss);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddArc(const Arc& a) {
  arcs_.push_back(a);
  node_arc_map[a.first].push_back(a);
  node_arc_map[a.second].push_back(a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddMatch(const Arc& a, double qty) {
  UpdateCapacity(a.first, a, qty);
  UpdateCapacity(a.second, a, qty);
  matches.push_back(std::make_pair(a, qty));
}

} // namespace cyclus
