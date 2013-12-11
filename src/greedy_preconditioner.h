#ifndef CYCLUS_GREEDY_PRECONDITIONER_H_
#define CYCLUS_GREEDY_PRECONDITIONER_H_

#include <map>
#include <string>

#include "exchange_graph.h"

namespace cyclus {

/// @returns the node's weight given the node and commodity weight
double NodeWeight(ExchangeNode::Ptr n,
                  std::map<std::string, double>* weights);

/// @returns average RequestGroup weight
double GroupWeight(RequestGroup::Ptr g,
                   std::map<std::string, double>* weights);

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
/// Consider the following commodity-to-weight mapping: {"spam": 5, "eggs": 2}.
/// Now consider two RequestGroups with the following commodities:
///   #. g1 = {"eggs", "spam", "eggs"}
///   #. g2 = {"eggs", "spam"}
///
/// First, the groups will be ordered and averaged weights will be determined:
///   #. g1 = {"spam", "eggs", "eggs"}, weight = 9/3
///   #. g2 = {"spam", "eggs"}, weight = 7/2
///
/// Finally, the groups themselves will be ordered by average weight:
///   #. {g2, g1}
class GreedyPreconditioner {
 public:
  /// @brief the order of commodity weights
  enum WgtOrder {
    REVERSE, ///< a flag for commodity weights given in the reverse order, i.e.,
             ///lightest first
    END ///< default flag, indicating heaviest-first ordering
  };

  /// @brief constructor if weights are given in heaviest-first order
  /// @warning weights are assumed to be positive
  GreedyPreconditioner(const std::map<std::string, double>& commod_weights)
    : commod_weights_(commod_weights) {
    ProcessWeights_(END);
  }
  
  /// @brief constructor if weights may not be given in heaviest-first order
  /// @warning weights are assumed to be positive
  GreedyPreconditioner(const std::map<std::string, double>& commod_weights,
                       WgtOrder order)
    : commod_weights_(commod_weights) {
    ProcessWeights_(order);
  };

  /// @brief conditions the graph as described above
  /// @throws KeyError if a commodity is in the graph but not in the weight
  /// mapping
  void Condition(ExchangeGraph* graph);

  /// @brief a comparitor for ordering containers of ExchangeNode::Ptrs in
  /// descending order based on their commodity's weight
  inline bool NodeComp(const ExchangeNode::Ptr l,
                       const ExchangeNode::Ptr r) {
    return NodeWeight(l, &commod_weights_) > NodeWeight(r, &commod_weights_);
  }

  /// @brief a comparitor for ordering containers of Request::Ptrs in
  /// descending order based on their average commodity weight
  inline bool GroupComp(const RequestGroup::Ptr l,
                        const RequestGroup::Ptr r) {
    return group_weights_[l] > group_weights_[r];
  }

 private:
  /// @brief normalizes all weights to 1 and puts them in the heaviest-first
  /// direction
  void ProcessWeights_(WgtOrder order);
  
  std::map<std::string, double> commod_weights_;
  std::map<RequestGroup::Ptr, double> group_weights_;
};

} // namespace cyclus

#endif // ifndef CYCLUS_GREEDY_PRECONDITIONER_H_

