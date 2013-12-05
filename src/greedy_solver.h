#ifndef CYCLUS_GREEDY_SOLVER_H_
#define CYCLUS_GREEDY_SOLVER_H_

#include "exchange_solver.h"

#include "exchange_graph.h"

namespace cyclus {

/// @brief A comparison function for sorting a container of Arcs by the
/// requester's (unode's) preference, in decensing order (i.e., most preferred
/// Arc first)
inline bool ReqPrefComp(const Arc& l, const Arc& r) {
  return l.first->prefs[l] > r.first->prefs[r];
}
  
class ExchangeGraph;
  
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
class GreedySolver: public ExchangeSolver {
 public:
  GreedySolver() {};
  explicit GreedySolver(ExchangeGraph* g) : ExchangeSolver(g) {};
  virtual ~GreedySolver() {};

  /// @brief the GreedySolver solves an ExchangeGraph by iterating over each
  /// RequestGroup and matching requests with the minimum bids possible, starting
  /// from the beginning of the the respective request and bid containers.
  virtual void Solve();

 private:
  void GreedilySatisfySet_(RequestGroup::Ptr prs);
};

} // namespace cyclus

#endif // ifndef CYCLUS_GREEDY_SOLVER_H_
