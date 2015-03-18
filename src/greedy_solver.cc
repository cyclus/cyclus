#include "greedy_solver.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>

#include "cyc_limits.h"
#include "error.h"
#include "logger.h"

namespace cyclus {

void Capacity(cyclus::Arc const&, double, double) {};
void Capacity(boost::shared_ptr<cyclus::ExchangeNode>, cyclus::Arc const&,
              double) {};

GreedySolver::GreedySolver(bool exclusive_orders, GreedyPreconditioner* c)
    : conditioner_(c),
      ExchangeSolver(exclusive_orders) {}

GreedySolver::GreedySolver(bool exclusive_orders)
    : ExchangeSolver(exclusive_orders) {
  conditioner_ = new cyclus::GreedyPreconditioner();  
}

GreedySolver::GreedySolver(GreedyPreconditioner* c)
    : conditioner_(c),
      ExchangeSolver(true) {}

GreedySolver::GreedySolver() : ExchangeSolver(true) {
  conditioner_ = new cyclus::GreedyPreconditioner();  
}

GreedySolver::~GreedySolver() {
  if (conditioner_ != NULL)
    delete conditioner_;
}

void GreedySolver::Condition() {
  if (conditioner_ != NULL)
    conditioner_->Condition(graph_);
}

void GreedySolver::Init() {
 std::for_each(graph_->request_groups().begin(),
                graph_->request_groups().end(),
                std::bind1st(
                    std::mem_fun(&GreedySolver::GetCaps),
                    this));

  std::for_each(graph_->supply_groups().begin(),
                graph_->supply_groups().end(),
                std::bind1st(
                    std::mem_fun(&GreedySolver::GetCaps),
                    this));
}

double GreedySolver::SolveGraph() {
  double pseudo_cost = PseudoCost(); // from ExchangeSolver API
  Condition();
  obj_ = 0;
  unmatched_ = 0;
  n_qty_.clear();
  
  Init();
 
  std::for_each(graph_->request_groups().begin(),
                graph_->request_groups().end(),
                std::bind1st(
                    std::mem_fun(&GreedySolver::GreedilySatisfySet),
                    this));

  obj_ += unmatched_ * pseudo_cost;
  return obj_;
}

double GreedySolver::Capacity(const Arc& a, double u_curr_qty,
                               double v_curr_qty) {
  bool min = true;
  double ucap = Capacity(a.unode(), a, !min, u_curr_qty);
  double vcap = Capacity(a.vnode(), a, min, v_curr_qty);

  CLOG(cyclus::LEV_DEBUG1) << "Capacity for unode of arc: " << ucap;
  CLOG(cyclus::LEV_DEBUG1) << "Capacity for vnode of arc: " << vcap;
  CLOG(cyclus::LEV_DEBUG1) << "Capacity for arc         : "
                           << std::min(ucap, vcap);

  return std::min(ucap, vcap);
}

double GreedySolver::Capacity(ExchangeNode::Ptr n, const Arc& a, bool min_cap,
                               double curr_qty) {
  if (n->group == NULL) {
    throw cyclus::StateError("An notion of node capacity requires a nodegroup.");
  }

  if (n->unit_capacities[a].size() == 0) {
    return n->qty - curr_qty;
  }

  std::vector<double>& unit_caps = n->unit_capacities[a];
  const std::vector<double>& group_caps = grp_caps_[n->group];
  const std::vector<cap_t>& cap_types = cap_types_[n->group];
  std::vector<double> caps;
  double grp_cap, u_cap, cap;

  double stdmax = std::numeric_limits<double>::max();
  double ltcap = stdmax;
  double gtcap = std::numeric_limits<double>::min();
  for (int i = 0; i < unit_caps.size(); i++) {
    grp_cap = group_caps[i];
    u_cap = unit_caps[i];
    // special case for unlimited capacities
    cap = grp_cap == stdmax ? stdmax : grp_cap / u_cap;
    CLOG(cyclus::LEV_DEBUG1) << "Capacity for node: ";
    CLOG(cyclus::LEV_DEBUG1) << "   group capacity: " << grp_cap;
    CLOG(cyclus::LEV_DEBUG1) << "    unit capacity: " << u_cap;
    CLOG(cyclus::LEV_DEBUG1) << "         capacity: " << cap;

    switch(cap_types[i]) {
      case GTEQ:
        gtcap = std::min(gtcap, cap);
        break;
      case LTEQ:
        ltcap = std::max(ltcap, cap);
        break;
      default:
          std::stringstream ss;
          ss << "A capacity has a type of NONE.";
          throw ValueError(ss.str());
    }
  }

  cap = std::min(ltcap, gtcap);
  return std::min(cap, n->qty - curr_qty);
}

void GreedySolver::GetCaps(ExchangeNodeGroup::Ptr g) {
  for (int i = 0; i != g->nodes().size(); i++) {
    n_qty_[g->nodes()[i]] = 0;
  }
  grp_caps_[g.get()] = g->capacities();
  cap_types_[g.get()] = g->cap_types();
}

void GreedySolver::GreedilySatisfySet(RequestGroup::Ptr prs) {
  std::vector<ExchangeNode::Ptr>& nodes = prs->nodes();
  std::stable_sort(nodes.begin(), nodes.end(), AvgPrefComp);

  std::vector<ExchangeNode::Ptr>::iterator req_it = nodes.begin();
  double target = prs->qty();
  double match = 0;

  ExchangeNode::Ptr u, v;
  std::vector<Arc>::const_iterator arc_it;
  std::vector<Arc> sorted;
  double remain, tomatch, excl_val;

  CLOG(LEV_DEBUG1) << "Greedy Solving for " << target
                   << " amount of a resource.";

  while ((match <= target) && (req_it != nodes.end())) {
    // this if statement is needed because map.at() will throw if the key does
    // not exist, which is a corner case for when there is a request with no bid
    // arcs associated with it
    if (graph_->node_arc_map().count(*req_it) > 0) {
      const std::vector<Arc>& arcs = graph_->node_arc_map().at(*req_it);
      sorted = std::vector<Arc>(arcs);  // make a copy for now
      std::stable_sort(sorted.begin(), sorted.end(), ReqPrefComp);
      arc_it = sorted.begin();

      while ((match <= target) && (arc_it != sorted.end())) {
        remain = target - match;
        const Arc& a = *arc_it;
        u = a.unode();
        v = a.vnode();
        // capacity adjustment
        tomatch = std::min(remain, Capacity(a, n_qty_[u], n_qty_[v]));

        // exclusivity adjustment
        if (arc_it->exclusive()) {
          excl_val = a.excl_val();
          tomatch = (tomatch < excl_val) ? 0 : excl_val;
        }

        if (tomatch > eps()) {
          CLOG(LEV_DEBUG1) << "Greedy Solver is matching " << tomatch
                           << " amount of a resource.";
          UpdateCapacity(u, a, tomatch);
          UpdateCapacity(v, a, tomatch);
          n_qty_[u] += tomatch;
          n_qty_[v] += tomatch;
          graph_->AddMatch(a, tomatch);

          match += tomatch;
          UpdateObj(tomatch, u->prefs[a]);
        }
        ++arc_it;
      }  // while( (match =< target) && (arc_it != arcs.end()) )
    }  // if(graph_->node_arc_map().count(*req_it) > 0)
    ++req_it;
  }  // while( (match =< target) && (req_it != nodes.end()) )

  unmatched_ += target - match;
}

void GreedySolver::UpdateObj(double qty, double pref) {
  // updates minimizing object (i.e., 1/pref is a cost and the objective is cost
  // * flow)
  obj_ += qty / pref;
}

void GreedySolver::UpdateCapacity(ExchangeNode::Ptr n, const Arc& a,
                                  double qty) {
  using cyclus::IsNegative;
  using cyclus::ValueError;

  std::vector<double>& unit_caps = n->unit_capacities[a];
  std::vector<double>& caps = grp_caps_[n->group];
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
    std::stringstream ss;
    ss << "A bid for " << n->commod << " was set at " << n->qty
       << " but has been matched to a higher value " << qty
       << ". This could be due to a problem with your "
       << "bid portfolio constraints.";
    throw ValueError(ss.str());
  }
}

}  // namespace cyclus
