#include <algorithm>

#include "cyc_limits.h"
#include "error.h"

#include "exchange_graph.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ExchangeNode::ExchangeNode(double max_qty) : max_qty(max_qty), qty(0), group(NULL) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool operator==(const ExchangeNode& lhs, const ExchangeNode& rhs) {
  return (lhs.unit_capacities == rhs.unit_capacities &&
          lhs.max_qty == rhs.max_qty &&
          lhs.group == rhs.group);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeNodeGroup::AddExchangeNode(ExchangeNode::Ptr node) {
  node->group = this;
  nodes.push_back(node);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RequestGroup::RequestGroup(double qty) : qty(qty) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Capacity(const Arc& a) {
  double ucap = Capacity(a.first, a);
  double vcap = Capacity(a.second, a);
  return std::min(ucap, vcap);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Capacity(ExchangeNode& n, const Arc& a) {
  if (n.group == NULL) {
    throw cyclus::StateError("An notion of node capacity requires a nodegroup.");
  }

  if (n.unit_capacities[a].size() == 0) {
    return std::numeric_limits<double>::max();
  }

  std::vector<double>& unit_caps = n.unit_capacities[a];
  const std::vector<double>& group_caps = n.group->capacities;
  std::vector<double> caps;
  for (int i = 0; i < unit_caps.size(); i++) {
    caps.push_back(group_caps[i] / unit_caps[i]);
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

  if (n.group == NULL) {
    throw cyclus::StateError("An notion of node capacity requires a nodegroup.");
  }
  
  vector<double>& unit_caps = n.unit_capacities[a];
  vector<double>& caps = n.group->capacities;
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
void ExchangeGraph::AddRequestGroup(RequestGroup::Ptr prs) {
  request_groups_.push_back(prs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddSupplyGroup(ExchangeNodeGroup::Ptr pss) {
  supply_groups_.push_back(pss);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddArc(const Arc& a) {
  arcs_.push_back(a);
  node_arc_map_[a.first].push_back(a);
  node_arc_map_[a.second].push_back(a);
}


#include <iostream>
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddMatch(const Arc& a, double qty) {
  UpdateCapacity(a.first, a, qty);
  UpdateCapacity(a.second, a, qty);
  matches_.push_back(std::make_pair(a, qty));
}

} // namespace cyclus
