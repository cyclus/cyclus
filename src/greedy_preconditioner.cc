#include <numeric>
#include <algorithm> 

#include <boost/lambda/bind.hpp>

#include <assert.h>
#include <string>

#include "cyc_std.h"
#include "logger.h"

#include "greedy_preconditioner.h"

namespace l = boost::lambda;

namespace cyclus {

inline double SumPref(double total, std::pair<Arc, double> pref) {
  return total += pref.second;
};

double AvgPref(ExchangeNode::Ptr n) {
  std::map<Arc, double>& prefs = n->prefs;
  return std::accumulate(prefs.begin(), prefs.end(), 0.0, SumPref) /
      prefs.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GreedyPreconditioner::Condition(ExchangeGraph* graph) {
  avg_prefs_.clear();
  
  std::vector<RequestGroup::Ptr>& groups =
      const_cast<std::vector<RequestGroup::Ptr>&>(graph->request_groups());

  std::vector<RequestGroup::Ptr>::iterator it;
  for (it = groups.begin(); it != groups.end(); ++it) {
    std::vector<ExchangeNode::Ptr>& nodes =
        const_cast<std::vector<ExchangeNode::Ptr>&>((*it)->nodes());

    // get avg prefs
    for (int i = 0; i != nodes.size(); i++) {
      avg_prefs_[nodes[i]] = AvgPref(nodes[i]);
    }
    
    // sort nodes by weight
    std::sort(nodes.begin(),
              nodes.end(),
              l::bind(&GreedyPreconditioner::NodeComp, this, l::_1, l::_2));
    
    // get avg group weights
    group_weights_[*it] = GroupWeight(*it, &commod_weights_, &avg_prefs_); 
    CLOG(LEV_DEBUG1) << "Group weight value during graph preconditioning is "
                     << group_weights_[*it] << ".";
  }

  // sort groups by avg weight
  std::sort(groups.begin(),
            groups.end(),
            l::bind(&GreedyPreconditioner::GroupComp, this, l::_1, l::_2));

  // clear graph-specific state
  group_weights_.clear();
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
  
  std::map<std::string, double>::iterator it;
  switch(order) {
    case REVERSE:
      for (it = commod_weights_.begin();
           it != commod_weights_.end();
           ++it) {
        it->second = max + min - it->second; // reverses order
      }
      break;
    default: // do nothing
      break;
  }
     
  for (it = commod_weights_.begin();
       it != commod_weights_.end();
       ++it) {
    CLOG(LEV_INFO1) << "GreedyPreconditioner commodity weight value for "
                    << it->first
                    << " is " << it->second;
  }
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double GroupWeight(RequestGroup::Ptr g,
                   std::map<std::string, double>* weights,
                   std::map<ExchangeNode::Ptr, double>* avg_prefs) {
  std::vector<ExchangeNode::Ptr>& nodes = g->nodes();
  double sum = 0;

  ExchangeNode::Ptr n;
  for (int i = 0; i != nodes.size(); i++) {
    n = nodes[i];
    sum += NodeWeight(n, weights, (*avg_prefs)[n]);
  }
  return sum / nodes.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double NodeWeight(ExchangeNode::Ptr n,
                  std::map<std::string, double>* weights,
                  double avg_pref) {
  double commod_weight = (*weights)[n->commod];
  double node_weight = commod_weight * ( 1 + avg_pref / ( 1 + avg_pref));
  
  CLOG(LEV_DEBUG5) << "Determining node weight: ";
  CLOG(LEV_DEBUG5) << "       commodity weight: " << commod_weight;
  CLOG(LEV_DEBUG5) << "               avg pref: " << avg_pref; 
  CLOG(LEV_DEBUG5) << "            node weight: " << node_weight;

  return node_weight;
}

} // namespace cyclus
