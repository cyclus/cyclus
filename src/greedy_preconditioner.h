#ifndef CYCLUS_GREEDY_PRECONDITIONER_H_
#define CYCLUS_GREEDY_PRECONDITIONER_H_

#include <map>
#include <string>

#include "exchange_graph.h"

namespace cyclus {

/// @class GreedyPreconditioner
///
/// @brief A GreedyPreconditioner conditions an ExchangeGraph for a GreedySolver
/// by ordering the RequestGroups and ExchangeNodes within each RequestGroup
/// weighted by their commodity's importance. The Graph is conditioned in-place.
///
/// @section weighting Weighting
/// Weights are provided to the conditioner via its constructor. A larger weight
/// implies a higher level of importance for solving. First, the ExchangeNodes
/// of each RequestGroup are sorted according to their weights. Then, the
/// average weight of each RequestGroup is determined. Finally, each
/// RequestGroup is sorted according to their average weight.
///
/// @section example Example
/// Consider the following commodity-to-weight mapping: {"spam": 1, "eggs": 2}.
/// Now consider two RequestGroups with the following commodities:
///   #. g1 = {"eggs", "spam"}
///   #. g2 = {"eggs", "spam", "eggs"}
///
/// First, the groups will be ordered and averaged weights will be determined:
///   #. g1 = {"spam", "eggs"}, weight = 3/2
///   #. g2 = {"spam", "eggs", "eggs"}, weight = 5/3
///
/// Finally, the groups themselves will be ordered by average weight:
///   #. {g2, g1}
class GreedyPreconditioner {
 public:
  GreedyPreconditioner(std::map<std::string, double> commod_weights)
    : commod_weights_(commod_weights) {};

  /// @brief conditions the graph as described above
  /// @throws KeyError if a commodity is in the graph but not in the weight
  /// mapping
  void Condition(ExchangeGraph* graph);

  /// @brief a comparitor for ordering containers of ExchangeNode::Ptrs in
  /// descending order based on their commodity's weight
  inline bool CommodComp(const ExchangeNode::Ptr l,
                         const ExchangeNode::Ptr r) {
    return commod_weights_[l->commod] > commod_weights_[r->commod];
  }

  /// @brief a comparitor for ordering containers of Request::Ptrs in
  /// descending order based on their average commodity weight
  inline bool GroupComp(const RequestGroup::Ptr l,
                        const RequestGroup::Ptr r) {
    return group_weights_[l] > group_weights_[r];
  }

 private:
  std::map<std::string, double> commod_weights_;
  std::map<RequestGroup::Ptr, double> group_weights_;
};

/// @returns average RequestGroup weight
double GroupWeight(RequestGroup::Ptr, std::map<std::string, double>& weights);

} // namespace cyclus

#endif // ifndef CYCLUS_GREEDY_PRECONDITIONER_H_

