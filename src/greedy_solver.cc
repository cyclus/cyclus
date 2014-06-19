#include "greedy_solver.h"

#include <cassert>
#include <algorithm>
#include <functional>
#include <vector>

#include "cyc_limits.h"
#include "error.h"
#include "logger.h"

namespace cyclus {

double Capacity(const Arc& a, double u_curr_qty, double v_curr_qty) {
  bool min = true;
  double ucap = Capacity(a.unode(), a, !min, u_curr_qty);
  double vcap = Capacity(a.vnode(), a, min, v_curr_qty);

  CLOG(cyclus::LEV_DEBUG1) << "Capacity for unode of arc: " << ucap;
  CLOG(cyclus::LEV_DEBUG1) << "Capacity for vnode of arc: " << vcap;
  CLOG(cyclus::LEV_DEBUG1) << "Capacity for arc         : "
                           << std::min(ucap, vcap);
  
  return std::min(ucap, vcap);
}

double Capacity(ExchangeNode::Ptr n, const Arc& a, bool min_cap,
                double curr_qty) {
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

  if (min_cap) { // the smallest value is constraining (for bids)
    cap = *std::min_element(caps.begin(), caps.end());
  } else { // the largest value must be met (for requests)
    cap = *std::max_element(caps.begin(), caps.end());
  }
  return std::min(cap, n->qty - curr_qty);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GreedySolver::GreedySolver(bool exclusive_orders, GreedyPreconditioner* c) 
  : conditioner_(c),
    ExchangeSolver(exclusive_orders) { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GreedySolver::~GreedySolver() {
  if (conditioner_ != NULL)
    delete conditioner_;
}

void GreedySolver::Condition() {
  if (conditioner_ == NULL)
    conditioner_ = new GreedyPreconditioner(std::map<std::string, double>());
  
  conditioner_->Condition(graph_);

}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedySolver::SolveGraph() {
  Condition();
  
  n_qty_.clear();
  
  std::for_each(graph_->request_groups().begin(),
                graph_->request_groups().end(),
                std::bind1st(
                    std::mem_fun(&GreedySolver::Init_),
                    this));
  
  std::for_each(graph_->supply_groups().begin(),
                graph_->supply_groups().end(),
                std::bind1st(
                    std::mem_fun(&GreedySolver::Init_),
                    this));
    
  std::for_each(graph_->request_groups().begin(),
                graph_->request_groups().end(),
                std::bind1st(
                    std::mem_fun(&GreedySolver::GreedilySatisfySet_),
                    this));

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedySolver::Init_(ExchangeNodeGroup::Ptr g) {
  for (int i = 0; i != g->nodes().size(); i++) {
    n_qty_[g->nodes()[i]] = 0;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedySolver::GreedilySatisfySet_(RequestGroup::Ptr prs) { 

  std::vector<ExchangeNode::Ptr>& nodes = prs->nodes();
  std::sort(nodes.begin(), nodes.end(), AvgPrefComp);
  
  std::vector<ExchangeNode::Ptr>::iterator req_it = nodes.begin();
  double target = prs->qty();
  double match = 0;
  
  ExchangeNode::Ptr u, v;
  std::vector<Arc>::const_iterator arc_it;
  std::vector<Arc> sorted;
  double remain, tomatch, excl_val;
  
  CLOG(LEV_DEBUG1) << "Greedy Solving for " << target
                   << " amount of a resource.";

  while( (match <= target) && (req_it != nodes.end()) ) {
    // this if statement is needed because map.at() will throw if the key does
    // not exist, which is a corner case for when there is a request with no bid
    // arcs associated with it
    if (graph_->node_arc_map().count(*req_it) > 0) {
      const std::vector<Arc>& arcs = graph_->node_arc_map().at(*req_it);
      sorted = std::vector<Arc>(arcs); // make a copy for now
      std::sort(sorted.begin(), sorted.end(), ReqPrefComp);
      arc_it = sorted.begin();
    
      while( (match <= target) && (arc_it != sorted.end()) ) {
        remain = target - match;
        const Arc& a = *arc_it;
        u = a.unode();
        v = a.vnode();
        
        // capacity adjustment
        tomatch = std::min(remain, Capacity(a, n_qty_[u], n_qty_[v]));

        // exclusivity adjustment
        if (arc_it->exclusive()) {
          excl_val = a.excl_val();
          tomatch = (tomatch < excl_val) ? 0 : excl_val;
        }
          
        if (tomatch > eps()) {
          CLOG(LEV_DEBUG1) << "Greedy Solver is matching " << tomatch
                           << " amount of a resource.";
          UpdateCapacity_(u, a, tomatch);
          UpdateCapacity_(v, a, tomatch);
          n_qty_[u] += tomatch;
          n_qty_[v] += tomatch;
          graph_->AddMatch(a, tomatch);
          
          match += tomatch;
        }
        ++arc_it;
      } // while( (match =< target) && (arc_it != arcs.end()) )
    } // if(graph_->node_arc_map().count(*req_it) > 0)
    
    ++req_it;
  } // while( (match =< target) && (req_it != nodes.end()) )

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedySolver::UpdateCapacity_(ExchangeNode::Ptr n, const Arc& a, double qty) {
  using cyclus::IsNegative;
  using cyclus::ValueError;

  std::vector<double>& unit_caps = n->unit_capacities[a];
  std::vector<double>& caps = n->group->capacities();
  assert(unit_caps.size() == caps.size());
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

  if (IsNegative(n->qty - qty)) {
    std::stringstream ss;
    ss << "A bid for " << n->commod << " was set at " << n->qty
       << " but has been matched to a higher value " << qty
       << ". This could be due to a problem with your "
       << "bid portfolio constraints.";
    throw ValueError(ss.str());
  }
}

} // namespace cyclus
