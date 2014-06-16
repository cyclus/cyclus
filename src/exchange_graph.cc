#include <algorithm>

#include "cyc_limits.h"
#include "error.h"
#include "logger.h"

#include "exchange_graph.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ExchangeNode::ExchangeNode(double qty, bool exclusive, std::string commod)
  : qty(qty),
    exclusive(exclusive),
    commod(commod),
    group(NULL) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool operator==(const ExchangeNode& lhs, const ExchangeNode& rhs) {
  return (lhs.unit_capacities == rhs.unit_capacities &&
          lhs.qty == rhs.qty &&
          lhs.exclusive == rhs.exclusive &&
          lhs.group == rhs.group &&
          lhs.commod == rhs.commod);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Arc::Arc(boost::shared_ptr<ExchangeNode> unode,
         boost::shared_ptr<ExchangeNode> vnode)
    : unode_(unode),
      vnode_(vnode) {
  exclusive_ = unode->exclusive || vnode->exclusive;
  if (exclusive_) {
    double fqty = unode->qty;
    double sqty = vnode->qty;
    if (unode->exclusive && vnode->exclusive) {
      excl_val_ = fqty == sqty ? fqty : 0;
    } else if (unode->exclusive) {
      excl_val_ = sqty >= fqty ? fqty : 0;
    } else {
      excl_val_ = fqty >= sqty ? sqty : 0;
    }
  } else {
    excl_val_ = 0;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Arc::Arc(const Arc& other)
    : unode_(other.unode()),
      vnode_(other.vnode()),
      exclusive_(other.exclusive()),
      excl_val_(other.excl_val()) { }
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeNodeGroup::AddExchangeNode(ExchangeNode::Ptr node) {
  node->group = this;
  nodes_.push_back(node);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeNodeGroup::AddExclNode(ExchangeNode::Ptr node) {
  std::vector<ExchangeNode::Ptr> nodes;
  nodes.push_back(node);
  excl_node_groups_.push_back(nodes);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RequestGroup::RequestGroup(double qty) : qty_(qty) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RequestGroup::AddExchangeNode(ExchangeNode::Ptr node) {
  ExchangeNodeGroup::AddExchangeNode(node);
  if (node->exclusive) {
    ExchangeNodeGroup::AddExclNode(node);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Capacity(const Arc& a, double u_curr_qty, double v_curr_qty) {
  double ucap = Capacity(a.unode(), a, u_curr_qty);
  double vcap = Capacity(a.vnode(), a, v_curr_qty);

  CLOG(cyclus::LEV_DEBUG1) << "Capacity for unode of arc: " << ucap;
  CLOG(cyclus::LEV_DEBUG1) << "Capacity for vnode of arc: " << vcap;
  CLOG(cyclus::LEV_DEBUG1) << "Capacity for arc         : "
                           << std::min(ucap, vcap);
  
  return std::min(ucap, vcap);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Capacity(ExchangeNode::Ptr n, const Arc& a, double curr_qty) {
  if (n->group == NULL) {
    throw cyclus::StateError("An notion of node capacity requires a nodegroup.");
  }

  if (n->unit_capacities[a].size() == 0) {
    return n->qty - curr_qty;
  }

  std::vector<double>& unit_caps = n->unit_capacities[a];
  const std::vector<double>& group_caps = n->group->capacities();
  std::vector<double> caps;
  double grp_cap, u_cap, cap;

  for (int i = 0; i < unit_caps.size(); i++) {
    grp_cap = group_caps[i];
    u_cap = unit_caps[i];
    cap = grp_cap / u_cap;
    CLOG(cyclus::LEV_DEBUG1) << "Capacity for node: ";
    CLOG(cyclus::LEV_DEBUG1) << "   group capacity: " << grp_cap;
    CLOG(cyclus::LEV_DEBUG1) << "    unit capacity: " << u_cap;
    CLOG(cyclus::LEV_DEBUG1) << "         capacity: " << cap;
    
    // special case for unlimited capacities
    if (grp_cap == std::numeric_limits<double>::max()) {
      caps.push_back(std::numeric_limits<double>::max());
    } else {
      caps.push_back(cap);
    }
  }

  return std::min(*std::min_element(caps.begin(), caps.end()),
                  n->qty - curr_qty);
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
  node_arc_map_[a.unode()].push_back(a);
  node_arc_map_[a.vnode()].push_back(a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddMatch(const Arc& a, double qty) {
  matches_.push_back(std::make_pair(a, qty));
}

} // namespace cyclus
