#include "prog_translator.h"

#include <algorithm>

#include "CoinPackedVector.hpp"
#include "OsiSolverInterface.hpp"

#include "cyc_limits.h"
#include "exchange_graph.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
ProgTranslator::ProgTranslator(ExchangeGraph* g, OsiSolverInterface* iface,
                               bool exclusive)
    :  g_(g), iface_(iface), excl_(exclusive) {
  arc_offset_ = g_->arcs().size();
  int n_cols = arc_offset_ + g_->request_groups().size();
  ctx_.obj_coeffs.resize(n_cols);
  ctx_.col_ubs.resize(n_cols);
  ctx_.col_lbs.resize(n_cols);
  ctx_.m = CoinPackedMatrix(false, 0, 0);
  min_row_coeff_ = std::numeric_limits<double>::max();
  max_obj_coeff_ = 0;
  cost_add_ = 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void ProgTranslator::Translate() {
  // number of variables = number of arcs + 1 faux arc per request group
  int n_cols = g_->arcs().size() + g_->request_groups().size();
  ctx_.m.setDimensions(0, n_cols);

  bool request;
  std::vector<ExchangeNodeGroup::Ptr>& sgs = g_->supply_groups();
  for (int i = 0; i != sgs.size(); i++) {
    request = false;
    XlateGrp_(sgs[i].get(), request);
  }

  std::vector<RequestGroup::Ptr>& rgs = g_->request_groups();
  for (int i = 0; i != rgs.size(); i++) {
    request = true;
    XlateGrp_(rgs[i].get(), request);
  }

  // add each false arc
  double inf = iface_->getInfinity();
  double max_cost = max_obj_coeff_ / min_row_coeff_ + cost_add_;
  for (int i = g_->arcs().size(); i != arc_offset_; i++) {
    ctx_.obj_coeffs[i] = max_cost;
    ctx_.col_lbs[i] = 0;
    ctx_.col_ubs[i] = inf;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void ProgTranslator::Populate() {
  iface_->setObjSense(1.0); // minimize

  // load er up!
  iface_->loadProblem(ctx_.m, &ctx_.col_lbs[0], &ctx_.col_ubs[0],
                      &ctx_.obj_coeffs[0], &ctx_.row_lbs[0], &ctx_.row_ubs[0]);

  if (excl_) {
    std::vector<Arc>& arcs = g_->arcs();
    for (int i = 0; i != arcs.size(); i++) {
      Arc& a = arcs[i];
      if (a.exclusive()) {
        iface_->setInteger(g_->arc_ids()[a]);
      }
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void ProgTranslator::ToProg() {
  Translate();
  Populate();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void ProgTranslator::XlateGrp_(ExchangeNodeGroup* grp, bool request) {
  double inf = iface_->getInfinity();
  std::vector<double>& caps = grp->capacities();

  std::vector<CoinPackedVector> cap_rows;
  std::vector<CoinPackedVector> excl_rows;
  for (int i = 0; i != caps.size(); i++) {
    cap_rows.push_back(CoinPackedVector());
  }
    
  std::vector<ExchangeNode::Ptr>& nodes = grp->nodes();
  for (int i = 0; i != nodes.size(); i++) {
    std::map<Arc, std::vector<double> >& ucap_map = nodes[i]->unit_capacities;
    std::map<Arc, std::vector<double> >::iterator cap_it;

    // add each arc
    for (cap_it = ucap_map.begin(); cap_it != ucap_map.end(); ++cap_it) {
      const Arc& a = cap_it->first;
      std::vector<double>& ucaps = cap_it->second;
      int arc_id = g_->arc_ids()[a];

      // add each unit capacity coefficient
      for (int j = 0; j != ucaps.size(); j++) {
        double coeff = ucaps[j];
        if (excl_ && a.exclusive()) {
          coeff *= a.excl_val();
        }

        cap_rows[j].insert(arc_id, coeff);
        
        double compare = coeff;// / caps[j];
        if (min_row_coeff_ > compare) {
          min_row_coeff_ = compare;
        }
      }
     
      if (request) {
        // add obj coeff for arc
        double pref = nodes[i]->prefs[a];
        double col_ub = std::min(nodes[i]->max_qty, inf);
        double obj_coeff = a.exclusive() ? a.excl_val() / pref  : 1 / pref;
        if (max_obj_coeff_ < obj_coeff) {
          max_obj_coeff_ = obj_coeff;
        }
        ctx_.obj_coeffs[arc_id] = obj_coeff;
        ctx_.col_lbs[arc_id] = 0;
        ctx_.col_ubs[arc_id] = a.exclusive() ? 1 : col_ub;
      }
    }
  }

  int faux_id;
  if (request) {
    faux_id = arc_offset_++;
  }

  // add all capacity rows  
  for (int i = 0; i != cap_rows.size(); i++) {
    if (request) {
      cap_rows[i].insert(faux_id, 1.0); // faux arc
    }
    
    ctx_.row_lbs.push_back(request ? caps[i] : 0);
    ctx_.row_ubs.push_back(request ? inf : caps[i]);
    ctx_.m.appendRow(cap_rows[i]);
  }

  // add exclusive arcs
  std::vector< std::vector<ExchangeNode::Ptr> >& exngs =
      grp->excl_node_groups();
  for (int i = 0; i != exngs.size(); i++) {
    CoinPackedVector excl_row;
    std::vector<ExchangeNode::Ptr>& nodes = exngs[i];
    for (int j = 0; j != nodes.size(); j++) {
      std::vector<Arc>& arcs = g_->node_arc_map()[nodes[j]];
      for (int k = 0; k != arcs.size(); k++) {
        excl_row.insert(g_->arc_ids()[arcs[k]], 1.0);
      }
    }
    if (excl_row.getNumElements() > 0) {
      excl_rows.push_back(excl_row);
    }
  }

  // add all exclusive rows
  for (int i = 0; i != excl_rows.size(); i++) {
    ctx_.row_lbs.push_back(0.0);
    ctx_.row_ubs.push_back(1.0);
    ctx_.m.appendRow(excl_rows[i]);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void ProgTranslator::FromProg() {
  const double* sol = iface_->getColSolution();
  std::vector<Arc>& arcs = g_->arcs();
  double flow;
  for (int i = 0; i < arcs.size(); i++) {
    Arc& a = g_->arc_by_id().at(i);
    flow = sol[i];
    flow = (a.exclusive()) ? flow * a.excl_val() : flow;
    if (flow > cyclus::eps()) {
      g_->AddMatch(a, flow);
    }
  }
}

} // namespace cyclus
