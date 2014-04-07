#ifndef CYCLUS_GREEDY_SOLVER_H_
#define CYCLUS_GREEDY_SOLVER_H_

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

 protected:
  /// @brief the GreedySolver solves an ExchangeGraph by iterating over each
  /// RequestGroup and matching requests with the minimum bids possible, starting
  /// from the beginning of the the respective request and bid containers.
  virtual void Solve();

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

} // namespace cyclus

#endif // ifndef CYCLUS_GREEDY_SOLVER_H_
