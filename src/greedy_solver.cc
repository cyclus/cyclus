#include "greedy_solver.h"

#include <algorithm>
#include <functional>
#include <vector>

#include "logger.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedySolver::Solve() {
  std::for_each(graph_->request_groups().begin(),
                graph_->request_groups().end(),
                std::bind1st(
                    std::mem_fun(&GreedySolver::GreedilySatisfySet_),
                    this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedySolver::GreedilySatisfySet_(RequestGroup::Ptr prs) { 
  double target = prs->qty;
  double match = 0;
  std::vector<ExchangeNode::Ptr>& nodes = prs->nodes;
  std::vector<ExchangeNode::Ptr>::const_iterator req_it = nodes.begin();

  CLOG(LEV_DEBUG1) << "Greedy Solving for " << target
                   << " amount of a resource.";
  while( (match <= target) && (req_it != nodes.end()) ) {
    // this if statement is needed because map.at() will throw if the key does
    // not exist, which is a corner case for when there is a request with no bid
    // arcs associated with it
    if (graph_->node_arc_map().count(*req_it) > 0) {
      const std::vector<Arc>& arcs = graph_->node_arc_map().at(*req_it);
      std::vector<Arc>::const_iterator arc_it = arcs.begin();
    
      while( (match <= target) && (arc_it != arcs.end()) ) {
        double remain = target - match;
        double tomatch = std::min(remain, Capacity(*arc_it));
        if (tomatch > 0) {
          graph_->AddMatch(*arc_it, tomatch);
          match += tomatch;
        }
        ++arc_it;
      } // while( (match =< target) && (arc_it != arcs.end()) )
    } // if(graph_->node_arc_map().count(*req_it) > 0)
    
    ++req_it;
  } // while( (match =< target) && (req_it != nodes.end()) )
  
}

} // namespace cyclus
