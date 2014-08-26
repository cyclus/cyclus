#ifndef CYCLUS_SRC_GREEDY_SOLVER_H_
#define CYCLUS_SRC_GREEDY_SOLVER_H_

#include "exchange_graph.h"
#include "exchange_solver.h"
#include "greedy_preconditioner.h"

namespace cyclus {

/// @brief A comparison function for sorting a container of Arcs by the
/// requester's (unode's) preference, in decensing order (i.e., most preferred
/// Arc first). In the case of a tie, a lexicalgraphic ordering of node ids is
/// used.
inline bool ReqPrefComp(const Arc* l, const Arc* r) {
  int lu = l->unode()->agent_id;
  int lv = l->vnode()->agent_id;
  int ru = r->unode()->agent_id;
  int rv = r->vnode()->agent_id;
  double lpref = l->unode()->prefs[l];
  double rpref = r->unode()->prefs[r];
  return (lpref != rpref) ? (lpref > rpref) : (lu > ru || (lu == ru && lv > rv));
}

/// @brief A comparison function for sorting a container of Nodes by the nodes
/// preference in decensing order (i.e., most preferred Node first). In the case
/// of a tie, a lexicalgraphic ordering of node ids is used.
inline bool AvgPrefComp(ExchangeNode::Ptr l, ExchangeNode::Ptr r) {
  int lid = l->agent_id;
  int rid = r->agent_id;
  double lpref =  AvgPref(l);
  double rpref = AvgPref(r);
  return (lpref != rpref) ? (lpref > rpref) : (lid > rid);
}

class ExchangeGraph;
class GreedyPreconditioner;

/// @brief The GreedySolver provides the implementation for a "greedy" solution
/// to a resource exchange graph.
///
/// Given an ExchangeGraph, the greedy solver will march through each
/// RequestGroup in the graph, matching request nodes "greedily" with supply
/// nodes. Each request node will attempt to be supplied by supplier arcs as
/// long as those supplier arcs have some excess capacity. The possible
/// suppliers will be ordered by descending preference. The algorithm terminates
/// when one of the following conditions is met:
///   1) All RequestGroups are satisfied
///   2) All SupplySets are at capacity
///
/// @warning the GreedySolver is responsible for deleting is conditioner!
class GreedySolver: public ExchangeSolver {
 public:
  /// GreedySolver constructor
  /// @param exclusive_orders a flag for enforcing integral, quantized orders
  /// @param c a conditioner to use before solving a graph instance
  /// @warning if a NULL pointer is passed as a conditioner argument,
  /// conditioning will *NOT* occur
  /// @{
  GreedySolver();
  explicit GreedySolver(bool exclusive_orders);
  explicit GreedySolver(GreedyPreconditioner* c);
  GreedySolver(bool exclusive_orders, GreedyPreconditioner* c);
  /// @}
  
  virtual ~GreedySolver();

  /// Uses the provided (or a default) GreedyPreconditioner to condition the
  /// solver's ExchangeGraph so that RequestGroups are ordered by average
  /// preference and commodity weight.
  ///
  /// @warning this function is called during the Solve step and should most
  /// likely not be called independently thereof (except for testing)
  void Condition();

  /// Initialize member values based on the given graph.
  void Init();

  /// @brief the capacity of the arc
  ///
  /// @throws StateError if either ExchangeNode does not have a ExchangeNodeGroup
  /// @param a the arc
  /// @param u_curr_qty the current quantity assigned to the unode (if solving
  /// piecemeal)
  /// @param v_curr_qty the current quantity assigned to the vnode (if solving
  /// piecemeal)
  /// @return The minimum of the unode and vnode's capacities
  // @{
  double Capacity(const Arc* a, double u_curr_qty, double v_curr_qty);
  inline double Capacity(const Arc* a) { return Capacity(a, 0, 0); }
  // @}

  /// @brief the capacity of a node
  ///
  /// @throws StateError if ExchangeNode does not have a ExchangeNodeGroup
  /// @param n the node
  /// @param min_cap whether to use the minimum or maximum capacity value. In general,
  /// nodes that represent bids use the minimum (i.e., the capacities represents a
  /// less-than constraint) and nodes that represent requests use the maximum
  /// value (i.e., the capacities represents a greater-than constraint).
  /// @param curr_qty the currently allocated node quantity (if solving piecemeal)
  /// @return The minimum of the node's nodegroup capacities / the node's unit
  /// capacities, or the ExchangeNode's remaining qty -- whichever is smaller.
  /// @{
  double Capacity(ExchangeNode::Ptr n, const Arc* a, bool min_cap,
                  double curr_qty);
  inline double Capacity(ExchangeNode::Ptr n, const Arc* a, bool min_cap) {
    return Capacity(n, a, min_cap, 0.0);
  }
  inline double Capacity(ExchangeNode::Ptr n, const Arc* a, double curr_qty) {
    return Capacity(n, a, true, curr_qty);
  }
  inline double Capacity(ExchangeNode::Ptr n, const Arc* a) {
    return Capacity(n, a, true, 0.0);
  }
  /// @}

 protected:
  /// @brief the GreedySolver solves an ExchangeGraph by iterating over each
  /// RequestGroup and matching requests with the minimum bids possible, starting
  /// from the beginning of the the respective request and bid containers.
  virtual double SolveGraph();

 private:
  /// @brief updates the capacity of a given ExchangeNode (i.e., its max_qty and the
  /// capacities of its ExchangeNodeGroup)
  ///
  /// @throws StateError if ExchangeNode does not have a ExchangeNodeGroup
  /// @throws ValueError if the update results in a negative ExchangeNodeGroup
  /// capacity or a negative ExchangeNode max_qty
  /// @param n the ExchangeNode
  /// @param qty the quantity for the node to update
  void GetCaps(ExchangeNodeGroup::Ptr prs);
  void GreedilySatisfySet(RequestGroup::Ptr prs);
  void UpdateCapacity(ExchangeNode::Ptr n, const Arc* a, double qty);
  void UpdateObj(double qty, double pref);
  
  GreedyPreconditioner* conditioner_;
  std::map<ExchangeNode::Ptr, double> n_qty_;
  std::map<ExchangeNodeGroup*, std::vector<double> > grp_caps_;
  double obj_;
  double unmatched_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_GREEDY_SOLVER_H_
