#include <numeric>
#include <algorithm> 

#include <boost/lambda/bind.hpp>

#include <assert.h>
#include <string>

#include "cyc_std.h"

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

    // sort nodes by weight
    std::sort(nodes.begin(),
              nodes.end(),
              l::bind(&GreedyPreconditioner::NodeComp, this, l::_1, l::_2));
    
    // get avg group weights
    group_weights_[*it] = GroupWeight(*it, &commod_weights_); 
  }

  // sort groups by avg weight
  std::sort(groups.begin(),
            groups.end(),
            l::bind(&GreedyPreconditioner::GroupComp, this, l::_1, l::_2));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedyPreconditioner::ProcessWeights_(WgtOrder order) {

  double min = std::min_element(
      commod_weights_.begin(),
      commod_weights_.end(),
      SecondLT< std::pair<std::string, double> >())->second;
  
  double max = std::max_element(
      commod_weights_.begin(),
      commod_weights_.end(),
      SecondLT< std::pair<std::string, double> >())->second;
  
  assert(min >= 0);
  
  switch(order) {
    case REVERSE:
      std::map<std::string, double>::iterator it;
      for (it = commod_weights_.begin();
           it != commod_weights_.end();
           ++it) {
        it->second = max + min - it->second; // reverses order
      }
  }
  
  // @MJGFlag not sure if this is needed..
  // std::map<std::string, double>::iterator it;
  // for (it = commod_weights_.begin();
  //      it != commod_weights_.end();
  //      ++it) {
  //   it->second /= min; // normalize to 1
  // }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double GroupWeight(RequestGroup::Ptr g,
                   std::map<std::string, double>* weights) {
  const std::vector<ExchangeNode::Ptr>& nodes = g->nodes();
  double sum = 0;
  std::vector<ExchangeNode::Ptr>::const_iterator it;
  for (it = nodes.begin(); it != nodes.end(); ++it) {
    sum += NodeWeight(*it, weights);
  }
  return sum / nodes.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double NodeWeight(ExchangeNode::Ptr n,
                  std::map<std::string, double>* weights) {
  return (*weights)[n->commod] * ( 1 + n->avg_pref / ( 1 + n->avg_pref));
}

} // namespace cyclus
