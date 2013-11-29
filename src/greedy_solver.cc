#include "greedy_solver.h"

#include <algorithm>
#include <functional>
#include <vector>

#include "logger.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedySolver::Solve() {
  std::for_each(graph_->request_sets.begin(),
                graph_->request_sets.end(),
                std::bind1st(
                    std::mem_fun(&GreedySolver::GreedilySatisfySet_),
                    this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedySolver::GreedilySatisfySet_(RequestSet::Ptr prs) { 
  double target = prs->qty;
  double match = 0;
  std::vector<Node::Ptr>& nodes = prs->nodes;
  std::vector<Node::Ptr>::iterator req_it = nodes.begin();

  CLOG(LEV_DEBUG1) << "Greedy Solving for " << target
                   << " amount of a resource.";
  while( (match <= target) && (req_it != nodes.end()) ) {
    std::vector<Arc>& arcs = graph_->node_arc_map[*req_it];
    std::vector<Arc>::iterator arc_it = arcs.begin();
    
    while( (match <= target) && (arc_it != arcs.end()) ) {
      double remain = target - match;
      double tomatch = std::min(remain, Capacity(*arc_it));
      if (tomatch > 0) {
        graph_->AddMatch(*arc_it, tomatch);
        match += tomatch;
      }
      ++arc_it;
    } // while( (match =< target) && (arc_it != arcs.end()) )
    
    ++req_it;
  } // while( (match =< target) && (req_it != nodes.end()) )
  
}

} // namespace cyclus
