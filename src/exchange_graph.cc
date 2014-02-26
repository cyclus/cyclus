#include <algorithm>

#include "cyc_limits.h"
#include "error.h"
#include "logger.h"

#include "exchange_graph.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ExchangeNode::ExchangeNode(double max_qty, bool exclusive = false)
  : max_qty(max_qty),
    exclusive(exclusive),
    qty(0),
    avg_pref(0),
    commod(""),
    group(NULL) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool operator==(const ExchangeNode& lhs, const ExchangeNode& rhs) {
  return (lhs.unit_capacities == rhs.unit_capacities &&
          lhs.max_qty == rhs.max_qty &&
          lhs.exclusive == rhs.exclusive &&
          lhs.group == rhs.group);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeNodeGroup::AddExchangeNode(ExchangeNode::Ptr node) {
  node->group = this;
  nodes_.push_back(node);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RequestGroup::RequestGroup(double qty) : qty_(qty) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Capacity(const Arc& a) {
  double ucap = Capacity(a.first, a);
  double vcap = Capacity(a.second, a);

  CLOG(cyclus::LEV_DEBUG1) << "Capacity for unode of arc: " << ucap;
  CLOG(cyclus::LEV_DEBUG1) << "Capacity for vnode of arc: " << vcap;
  CLOG(cyclus::LEV_DEBUG1) << "Capacity for arc         : "
                           << std::min(ucap, vcap);
  
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
  const std::vector<double>& group_caps = n.group->capacities();
  std::vector<double> caps;
  for (int i = 0; i < unit_caps.size(); i++) {
    CLOG(cyclus::LEV_DEBUG1) << "Capacity for node: ";
    CLOG(cyclus::LEV_DEBUG1) << "   group capacity: " << group_caps[i];
    CLOG(cyclus::LEV_DEBUG1) << "    unit capacity: " << unit_caps[i];
    CLOG(cyclus::LEV_DEBUG1) << "         capacity: "
                             << group_caps[i] / unit_caps[i];
    
    // special case for unlimited capacities
    if (group_caps[i] == std::numeric_limits<double>::max()) {
      caps.push_back(std::numeric_limits<double>::max());
    } else {
      caps.push_back(group_caps[i] / unit_caps[i]);
    }
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
  vector<double>& caps = n.group->capacities();
  for (int i = 0; i < caps.size(); i++) {
    double prev = caps[i];
    // special case for unlimited capacities
    CLOG(cyclus::LEV_DEBUG1) << "Updating capacity value from: "
                             << prev;
    caps[i] = (prev == std::numeric_limits<double>::max()) ?
              std::numeric_limits<double>::max() :
              prev - qty * unit_caps[i];
    CLOG(cyclus::LEV_DEBUG1) << "                          to: "
                             << caps[i];
  }

  if (IsNegative(n.max_qty - qty)) {
    throw ValueError("ExchangeNode quantities can not be reduced below 0.");
  }

  n.qty += qty;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UpdateCapacity(ExchangeNode::Ptr pn, const Arc& a, double qty) {
  return UpdateCapacity(*pn.get(), a, qty);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ExchangeGraph::ExchangeGraph() : next_arc_id_(0) { }

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
  int id = next_arc_id_++;
  arc_ids_.insert(std::pair<Arc, int>(a, id));
  arc_by_id_.insert(std::pair<int, Arc>(id, a));
  node_arc_map_[a.first].push_back(a);
  node_arc_map_[a.second].push_back(a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddMatch(const Arc& a, double qty) {
  UpdateCapacity(a.first, a, qty);
  UpdateCapacity(a.second, a, qty);
  matches_.push_back(std::make_pair(a, qty));
}

} // namespace cyclus
