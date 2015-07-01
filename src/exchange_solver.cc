#include "exchange_solver.h"

#include <vector>
#include <map>

#include "exchange_graph.h"

namespace cyclus {

double ExchangeSolver::PseudoCost() {
  return PseudoCost(1);
}

double ExchangeSolver::PseudoCost(double cost_add) {
  return PseudoCostByPref(cost_add);
}

double ExchangeSolver::PseudoCostByCap(double cost_add) {
  std::vector<ExchangeNode::Ptr>::iterator n_it;
  std::map<Arc, std::vector<double> >::iterator c_it;
  std::map<Arc, double>::iterator p_it;
  std::vector<RequestGroup::Ptr>::iterator rg_it;
  std::vector<ExchangeNodeGroup::Ptr>::iterator sg_it;
  double min_cap, pref, coeff;

  double max_coeff = std::numeric_limits<double>::min();
  double min_unit_cap = std::numeric_limits<double>::max();

  for (sg_it = graph_->supply_groups().begin();
       sg_it != graph_->supply_groups().end();
       ++sg_it) {
    std::vector<ExchangeNode::Ptr>& nodes = (*sg_it)->nodes();
    for (n_it = nodes.begin(); n_it != nodes.end(); ++n_it) {
      // update min_unit_cap
      std::map<Arc, std::vector<double> >::iterator c_it;
      std::map<Arc, std::vector<double> >& caps = (*n_it)->unit_capacities;
      for (c_it = caps.begin(); c_it != caps.end(); ++c_it) {
        std::vector<double>& ucaps = c_it->second; 
        if (!ucaps.empty()) {
          min_cap = *std::min_element(ucaps.begin(), ucaps.end());
          if (min_cap < min_unit_cap)
            min_unit_cap = min_cap;
        }
      }
    }
  }

  for (rg_it = graph_->request_groups().begin();
       rg_it != graph_->request_groups().end();
       ++rg_it) {
    std::vector<ExchangeNode::Ptr>& nodes = (*rg_it)->nodes();
    for (n_it = nodes.begin(); n_it != nodes.end(); ++n_it) {
      // update min_unit_cap
      std::map<Arc, std::vector<double> >::iterator c_it;
      std::map<Arc, std::vector<double> >& caps = (*n_it)->unit_capacities;
      for (c_it = caps.begin(); c_it != caps.end(); ++c_it) {
        std::vector<double>& ucaps = c_it->second; 
        if (!ucaps.empty()) {
          min_cap = *std::min_element(ucaps.begin(), ucaps.end());
          if (min_cap < min_unit_cap)
            min_unit_cap = min_cap;
        }
      }
      
      // update max_pref_
      std::map<Arc, double>& prefs = (*n_it)->prefs;
      for (p_it = prefs.begin(); p_it != prefs.end(); ++p_it) {
        pref = p_it->second;
        const Arc& a = p_it->first;
        coeff = (exclusive_orders_ && a.exclusive()) ?
                a.excl_val() / pref : 1.0 / pref;
        if (coeff > max_coeff)
          max_coeff = coeff;
      }
    }
  }

  return max_coeff / min_unit_cap + cost_add;
}

double ExchangeSolver::PseudoCostByPref(double cost_add) {
  double cost;
  double max_cost = 0;
  std::vector<Arc>& arcs = graph_->arcs();
  for (int i = 0; i != arcs.size(); i++) {
    Arc& a = arcs[i];
    if (!a.exclusive()) {
      cost = 1 / a.pref();
    } else {
      cost = a.unode()->qty / a.pref();
      // special case for small exclusive quantities
      // cost add must satisfy x > 1/ p * (1 - q)
      // guarantee strict greater than by multiply by an increase factor
      // (e.g., 5% -> multiply by 1.05)
      if (a.unode()->qty < 1) 
        cost_add = std::max(cost_add, 1.05 * (1 - a.unode()->qty) / a.pref());
    }
    max_cost = std::max(max_cost, cost);
    std::cout << "bar: " << max_cost << "\n";
  }
  return max_cost + cost_add;
}

} // namespace cyclus
