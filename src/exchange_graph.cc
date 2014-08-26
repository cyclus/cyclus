#include "exchange_graph.h"

#include <algorithm>

#include "cyc_limits.h"
#include "error.h"
#include "logger.h"

namespace cyclus {

ExchangeNode::ExchangeNode(double qty, bool exclusive, std::string commod,
                           int agent_id)
    : qty(qty),
      exclusive(exclusive),
      commod(commod),
      agent_id(agent_id),
      group(NULL) {}

ExchangeNode::ExchangeNode(double qty, bool exclusive)
    : qty(qty),
      exclusive(exclusive),
      commod(""),
      agent_id(-1),
      group(NULL) {}

ExchangeNode::ExchangeNode(double qty)
    : qty(qty),
      exclusive(false),
      commod(""),
      agent_id(-1),
      group(NULL) {}

ExchangeNode::ExchangeNode()
    : qty(std::numeric_limits<double>::max()),
      exclusive(false),
      commod(""),
      agent_id(-1),
      group(NULL) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool operator==(const ExchangeNode& lhs, const ExchangeNode& rhs) {
  return (lhs.unit_capacities == rhs.unit_capacities &&
          lhs.qty == rhs.qty &&
          lhs.exclusive == rhs.exclusive &&
          lhs.group == rhs.group &&
          lhs.commod == rhs.commod);
}

int Arc::index_ = 0;
std::vector<Arc*> Arc::cache_;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Arc::Arc(boost::shared_ptr<ExchangeNode> unode,
         boost::shared_ptr<ExchangeNode> vnode) {
  init(unode, vnode);
}

Arc* Arc::Make(boost::shared_ptr<ExchangeNode> unode,
          boost::shared_ptr<ExchangeNode> vnode) {
  if (cache_.size() == 0) {
    index_ = 0;
    for (int i = 0; i < 100; i++) {
      cache_.push_back(new Arc());
    }
  } else if (index_ == cache_.size()) {
    cache_.resize(cache_.size() * 2, NULL);
    for (int i = index_; i < cache_.size(); i++) {
      cache_[i] = new Arc();
    }
  }

  return cache_[index_++]->init(unode, vnode);
}

Arc* Arc::init(boost::shared_ptr<ExchangeNode> unode,
           boost::shared_ptr<ExchangeNode> vnode) {
  unode_ = unode;
  vnode_ = vnode;
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
  return this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Arc::Arc(const Arc& other)
    : unode_(other.unode()),
      vnode_(other.vnode()),
      exclusive_(other.exclusive()),
      excl_val_(other.excl_val()) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeNodeGroup::AddExchangeNode(ExchangeNode::Ptr node) {
  node->group = this;
  nodes_.push_back(node);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeNodeGroup::AddExclNode(ExchangeNode::Ptr node) {
  std::vector<ExchangeNode::Ptr> nodes;
  nodes.push_back(node);
  excl_node_groups_.push_back(nodes);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RequestGroup::RequestGroup(double qty) : qty_(qty) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RequestGroup::AddExchangeNode(ExchangeNode::Ptr node) {
  ExchangeNodeGroup::AddExchangeNode(node);
  if (node->exclusive) {
    ExchangeNodeGroup::AddExclNode(node);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ExchangeGraph::ExchangeGraph() : next_arc_id_(0) { }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddRequestGroup(RequestGroup::Ptr prs) {
  request_groups_.push_back(prs);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddSupplyGroup(ExchangeNodeGroup::Ptr pss) {
  supply_groups_.push_back(pss);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddArc(const Arc* a) {
  arcs_.push_back(a);
  int id = next_arc_id_++;
  arc_ids_.insert(std::pair<const Arc*, int>(a, id));
  arc_by_id_.insert(std::pair<int, const Arc*>(id, a));
  node_arc_map_[a->unode()].push_back(a);
  node_arc_map_[a->vnode()].push_back(a);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void ExchangeGraph::AddMatch(const Arc* a, double qty) {
  matches_.push_back(std::make_pair(a, qty));
}

}  // namespace cyclus
