#include "greedy_solver.h"

#include <cassert>
#include <algorithm>
#include <functional>
#include <vector>

#include "cyc_limits.h"
#include "error.h"
#include "logger.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GreedySolver::GreedySolver(bool exclusive_orders, GreedyPreconditioner* c) 
  : conditioner_(c),
    ExchangeSolver(exclusive_orders) { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GreedySolver::~GreedySolver() {
  if (conditioner_ != NULL)
    delete conditioner_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedySolver::Solve() {
  if (conditioner_ != NULL) {
    conditioner_->Condition(graph_);
  }

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

  const std::vector<ExchangeNode::Ptr>& nodes = prs->nodes();
  std::vector<ExchangeNode::Ptr>::const_iterator req_it = nodes.begin();
  double target = prs->qty();
  double match = 0;
  
  ExchangeNode::Ptr u, v;
  std::vector<Arc>::const_iterator arc_it;
  std::vector<Arc> sorted;
  double remain, tomatch, node_max, excl_val;
  
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
        node_max = std::min(Capacity(u, a, n_qty_[u]),
                            Capacity(v, a, n_qty_[v]));
        tomatch = std::min(remain, node_max);

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
    throw ValueError("ExchangeNode quantities can not be reduced below 0.");
  }
}

} // namespace cyclus
