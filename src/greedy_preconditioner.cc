#include <numeric>

#include <boost/lambda/bind.hpp>

#include "greedy_preconditioner.h"

namespace l = boost::lambda;

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedyPreconditioner::Condition(ExchangeGraph* graph) {
  std::vector<RequestGroup::Ptr>& groups =
      const_cast<std::vector<RequestGroup::Ptr>&>(graph->request_groups());

  std::vector<RequestGroup::Ptr>::iterator it;
  for (it = groups.begin(); it != groups.end(); ++it) {
    std::vector<ExchangeNode::Ptr>& nodes =
        const_cast<std::vector<ExchangeNode::Ptr>&>((*it)->nodes());

    // sort nodes by commod weight
    std::sort(nodes.begin(),
              nodes.end(),
              l::bind(&GreedyPreconditioner::CommodComp, this, l::_1, l::_2));
    
    // get avg group weights
    group_weights_[*it] = GroupWeight(*it, commod_weights_); 
  }

  // sort groups by avg weight
  std::sort(groups.begin(),
            groups.end(),
            l::bind(&GreedyPreconditioner::GroupComp, this, l::_1, l::_2));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double GroupWeight(RequestGroup::Ptr g,
                   std::map<std::string, double>& weights) {
  const std::vector<ExchangeNode::Ptr>& nodes = g->nodes();
  double sum = 0;
  std::vector<ExchangeNode::Ptr>::const_iterator it;
  for (it = nodes.begin(); it != nodes.end(); ++it) {
    sum += weights[(*it)->commod];
  }
  return sum / nodes.size();
}

} // namespace cyclus
