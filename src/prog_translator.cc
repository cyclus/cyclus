#include "prog_translator.h"

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
  int reserve = arc_offset_ + g_->request_groups().size();
  ctx_.obj_coeffs.reserve(reserve);
  ctx_.col_ubs.reserve(reserve);
  ctx_.col_lbs.reserve(reserve);
  ctx_.m = CoinPackedMatrix(false, 0, 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void ProgTranslator::Translate() {
  // number of variables = number of arcs + 1 faux arc per request group
  int n_cols = g_->arcs().size() + g_->request_groups().size();
  ctx_.m.setDimensions(0, n_cols);

  bool req;
  std::vector<ExchangeNodeGroup::Ptr>& sgs = g_->supply_groups();
  for (int i = 0; i != sgs.size(); i++) {
    req = false;
    XlateGrp_(sgs[i].get(), req);
  }

  std::vector<RequestGroup::Ptr>& rgs = g_->request_groups();
  for (int i = 0; i != rgs.size(); i++) {
    req = true;
    XlateGrp_(rgs[i].get(), req);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void ProgTranslator::Populate() {
  iface_->setObjSense(-1.0); // maximize

  // load er up!
  iface_->loadProblem(ctx_.m, &ctx_.col_lbs[0], &ctx_.col_ubs[0],
                      &ctx_.obj_coeffs[0], &ctx_.row_lbs[0], &ctx_.row_ubs[0]);

  if (excl_) {
    std::vector<Arc>& arcs = g_->arcs();
    for (int i = 0; i != arcs.size(); i++) {
      Arc& a = arcs[i];
      if (a.exclusive) {
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
void ProgTranslator::XlateGrp_(ExchangeNodeGroup* grp, bool req) {
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
    CoinPackedVector excl_row;

    // add each arc
    for (cap_it = ucap_map.begin(); cap_it != ucap_map.end(); ++cap_it) {
      const Arc& a = cap_it->first;
      std::vector<double>& ucaps = cap_it->second;
      int arc_id = g_->arc_ids()[a];

      // add each unit capacity coefficient
      for (int j = 0; j != ucaps.size(); j++) {
        double coeff = ucaps[j];
        if (excl_ && a.exclusive) {
          coeff *= a.excl_val;
        }
        cap_rows[j].insert(arc_id, coeff);
      }
      
      // add exclusive arc
      if (excl_ && req && a.exclusive) {
        excl_row.insert(arc_id, 1.0);
      }

      if (req) {
        // add obj coeff for arc
        double pref = nodes[i]->prefs[a];
        pref = a.exclusive ? pref * a.excl_val : pref;
        ctx_.obj_coeffs[arc_id] = pref;
        ctx_.col_lbs[arc_id] = 0;
        ctx_.col_ubs[arc_id] = a.exclusive ? 1 : inf;
      }
    }
    
    if (excl_row.getNumElements() > 0) {
      excl_rows.push_back(excl_row);
    }
  }

  int faux_id;
  if (req) {
    faux_id = arc_offset_++;
    ctx_.obj_coeffs[faux_id] = -1;
    ctx_.col_lbs[faux_id] = 0;
    ctx_.col_ubs[faux_id] = inf;
  }

  // add all capacity rows  
  for (int i = 0; i != cap_rows.size(); i++) {
    if (req) {
      cap_rows[i].insert(faux_id, 1.0); // faux arc
    }
    
    double lb = req ? caps[i] : 0;
    double ub = req ? inf : caps[i];
    ctx_.row_lbs.push_back(lb);
    ctx_.row_ubs.push_back(ub);
    ctx_.m.appendRow(cap_rows[i]);
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
  for (int i = 0; i < arcs.size(); i++) {
    double flow = sol[i];
    Arc& a = g_->arc_by_id().at(i);
    flow = (a.exclusive) ? flow * a.excl_val : flow;
    if (flow > cyclus::eps()) {
      g_->AddMatch(a, flow);
    }
  }
}

} // namespace cyclus
