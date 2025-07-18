#ifndef CYCLUS_SRC_GREEDY_PRECONDITIONER_H_
#define CYCLUS_SRC_GREEDY_PRECONDITIONER_H_

#include <map>
#include <string>

#include "exchange_graph.h"

namespace cyclus {

/// @returns the node's weight given the node and commodity weight
double NodeWeight(ExchangeNode::Ptr n,
                  std::map<std::string, double>* weights,
                  double avg_pref);

/// @returns average RequestGroup weight
double GroupWeight(RequestGroup::Ptr g,
                   std::map<std::string, double>* weights,
                   std::map<ExchangeNode::Ptr, double>* avg_prefs);

/// @returns the average preference across arcs for a node
double AvgPref(ExchangeNode::Ptr n);

/// @class GreedyPreconditioner
///
/// @brief A GreedyPreconditioner conditions an ExchangeGraph for a GreedySolver
/// by ordering the RequestGroups and ExchangeNodes within each RequestGroup
/// weighted by their commodity's importance. The Graph is conditioned in-place.
///
/// @section weighting Weighting
///
/// Commodity weights are provided to the conditioner via its constructor. A
/// larger weight implies a higher level of importance for solving.
///
/// The conditioning weight for a node is calculated as $w_cond_i = w_commod_i *
/// (1 + \frac{\overline{p_i}}{1 + \overline{p_i}})$, where $w_cond_i$ is the
/// calculated conditioning weight, $w_commod_i$ is node $i$'s commodity's
/// weight, and $\overline{p_i}$ is the average preference of all bid arcs
/// associated with node $i$.
///
/// First, the ExchangeNodes of each RequestGroup are sorted according to
/// their conditioning weights. Then, the average weight of each RequestGroup is
/// determined. Finally, each RequestGroup is sorted according to their average
/// weight.
///
/// @section example Example
/// Consider the following commodity-to-weight mapping: {"spam": 5, "eggs": 2}.
/// Now consider two RequestGroups with the following commodities:
///   #. g1 = {"eggs", "spam", "eggs"}
///   #. g2 = {"eggs", "spam"}
/// And the following preference-commodity  mapping:
/// {g1: {"spam": 3/4, "eggs": 1/4}, g2: {"spam": 1, "eggs": 1}.
///
/// First, the groups will be ordered by conditioning weights:
///   #. g1 = {"spam", "eggs", "eggs"}
///   #. g2 = {"spam", "eggs"}
///
/// Finally, the groups themselves will be ordered by average weight:
///   #. {g2, g1}
class GreedyPreconditioner {
 public:
  /// @brief the order of commodity weights
  enum WgtOrder {
    REVERSE,  /// a flag for commodity weights given in the reverse order,
              /// i.e, lightest first
    END       /// default flag, indicating heaviest-first ordering
  };

  /// @brief constructor if weights are given in heaviest-first order
  /// @warning weights are assumed to be positive
  /// @{
  GreedyPreconditioner();
  GreedyPreconditioner(const std::map<std::string, double>& commod_weights);
  /// @brief constructor if weights may not be given in heaviest-first order
  /// @warning weights are assumed to be positive
  GreedyPreconditioner(const std::map<std::string, double>& commod_weights,
                       WgtOrder order);
  /// @}

  /// @brief conditions the graph as described above
  /// @throws KeyError if a commodity is in the graph but not in the weight
  /// mapping
  void Condition(ExchangeGraph* graph);

  /// @brief a comparitor for ordering containers of ExchangeNode::Ptrs in
  /// descending order based on their commodity's weight
  inline bool NodeComp(const ExchangeNode::Ptr l, const ExchangeNode::Ptr r) {
    return NodeWeight(l, &commod_weights_, avg_prefs_[l]) >
           NodeWeight(r, &commod_weights_, avg_prefs_[r]);
  }

  /// @brief a comparitor for ordering containers of Request::Ptrs in
  /// descending order based on their average commodity weight
  inline bool GroupComp(const RequestGroup::Ptr l, const RequestGroup::Ptr r) {
    return group_weights_[l] > group_weights_[r];
  }

 private:
  /// @brief normalizes all weights to 1 and puts them in the heaviest-first
  /// direction
  void ProcessWeights_(WgtOrder order);

  bool apply_commod_weights_;
  std::map<ExchangeNode::Ptr, double> avg_prefs_;
  std::map<std::string, double> commod_weights_;
  std::map<RequestGroup::Ptr, double> group_weights_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_GREEDY_PRECONDITIONER_H_
