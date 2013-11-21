#include "greedy_solver.h"

#include <algorithm>
#include <functional>
#include <vector>

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedySolver::Solve() {
  std::for_each(graph_->request_sets.begin(),
                graph_->request_sets.end(),
                std::bind1st(
                    std::mem_fun(&GreedySolver::__GreedilySatisfySet),
                    this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedySolver::__GreedilySatisfySet(RequestSet::Ptr prs) { 
  double target = prs->qty;
  double match = 0;
  std::vector<Node::Ptr>& nodes = prs->nodes;
  std::vector<Node::Ptr>::iterator req_it = nodes.begin();
  
  while( (match <= target) && (req_it != nodes.end()) ) {
    std::vector<Arc::Ptr>& arcs = graph_->node_arc_map[*req_it];
    std::vector<Arc::Ptr>::iterator arc_it = arcs.begin();
    
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
