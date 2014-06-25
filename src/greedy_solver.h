#ifndef CYCLUS_SRC_GREEDY_SOLVER_H_
#define CYCLUS_SRC_GREEDY_SOLVER_H_

#include "exchange_graph.h"
#include "exchange_solver.h"
#include "greedy_preconditioner.h"

namespace cyclus {

/// @brief A comparison function for sorting a container of Arcs by the
/// requester's (unode's) preference, in decensing order (i.e., most preferred
/// Arc first)
inline bool ReqPrefComp(const Arc& l, const Arc& r) {
  return l.unode()->prefs[l] > r.unode()->prefs[r];
}

/// @brief A comparison function for sorting a container of Nodes by the nodes
/// preference in decensing order (i.e., most preferred Node first)
inline bool AvgPrefComp(ExchangeNode::Ptr l, ExchangeNode::Ptr r) {
  return AvgPref(l) > AvgPref(r);
}

class ExchangeGraph;
class GreedyPreconditioner;

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
double Capacity(const Arc& a, double u_curr_qty, double v_curr_qty);
inline double Capacity(const Arc& a) { return Capacity(a, 0, 0); }
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
// @{
double Capacity(ExchangeNode::Ptr n, const Arc& a, bool min_cap,
                double curr_qty);
inline double Capacity(ExchangeNode::Ptr n, const Arc& a, bool min_cap) {
  return Capacity(n, a, min_cap, 0.0);
}
inline double Capacity(ExchangeNode::Ptr n, const Arc& a, double curr_qty) {
  return Capacity(n, a, true, curr_qty);
}
inline double Capacity(ExchangeNode::Ptr n, const Arc& a) {
  return Capacity(n, a, true, 0.0);
}
// @}

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
  GreedySolver(bool exclusive_orders = false,
               GreedyPreconditioner* c = NULL);

  virtual ~GreedySolver();

  /// Uses the provided (or a default) GreedyPreconditioner to condition the
  /// solver's ExchangeGraph so that RequestGroups are ordered by average
  /// preference and commodity weight.
  ///
  /// @warning this function is called during the Solve step and should most
  /// likely not be called independently thereof (except for testing)
  void Condition();

 protected:
  /// @brief the GreedySolver solves an ExchangeGraph by iterating over each
  /// RequestGroup and matching requests with the minimum bids possible, starting
  /// from the beginning of the the respective request and bid containers.
  virtual void SolveGraph();

 private:
  /// @brief updates the capacity of a given ExchangeNode (i.e., its max_qty and the
  /// capacities of its ExchangeNodeGroup)
  ///
  /// @throws StateError if ExchangeNode does not have a ExchangeNodeGroup
  /// @throws ValueError if the update results in a negative ExchangeNodeGroup
  /// capacity or a negative ExchangeNode max_qty
  /// @param n the ExchangeNode
  /// @param qty the quantity for the node to update
  void Init_(ExchangeNodeGroup::Ptr prs);
  void GreedilySatisfySet_(RequestGroup::Ptr prs);
  void UpdateCapacity_(ExchangeNode::Ptr n, const Arc& a, double qty);

  GreedyPreconditioner* conditioner_;
  std::map<ExchangeNode::Ptr, double> n_qty_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_GREEDY_SOLVER_H_
