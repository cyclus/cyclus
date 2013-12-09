#ifndef CYCLUS_EXCHANGE_GRAPH_H_
#define CYCLUS_EXCHANGE_GRAPH_H_

#include <limits>
#include <map>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

namespace cyclus {

class ExchangeNodeGroup;
class ExchangeNode;

/// @brief by convention, arc.first == request node (unode), arc.second == bid
/// node (vnode)
typedef std::pair<
    boost::shared_ptr<ExchangeNode>,
    boost::shared_ptr<ExchangeNode> > Arc;

/// @class ExchangeNode
///
/// @brief ExchangeNodes are used in ExchangeGraphs to house information about a
/// given translated Bid or Request. Specifically, ExchangeNodes have a notion
/// of unit capacities that the given Bid or Request contribute to the overall
/// capacity of ExchangeNodeGroup. ExchangeNodes also have a notion of quantity,
/// i.e., the maximum amount of a resource that can be attributed to it.
struct ExchangeNode {
 public:
  typedef boost::shared_ptr<ExchangeNode> Ptr;
  
  ExchangeNode(double max_qty = std::numeric_limits<double>::max());

  /// @brief the parent ExchangeNodeGroup to which this ExchangeNode belongs
  ExchangeNodeGroup* group;

  /// @brief unit values associated with this ExchangeNode corresponding to
  /// capacties of its parent ExchangeNodeGroup. This information corresponds to
  /// the resource object from which this ExchangeNode was translated.
  std::map<Arc, std::vector<double> > unit_capacities;

  /// @brief preference values for arcs
  std::map<Arc, double> prefs;
  
  /// @brief the maximum amount of a resource that can be associated with this
  /// node
  double max_qty;

  /// @brief a running total of the amount of resource associated with this
  /// node
  double qty;

  /// @brief the commodity associated with this exchange node
  std::string commod;
};

/// @brief ExchangeNode-ExchangeNode equality operator
inline bool operator==(const ExchangeNode& lhs, const ExchangeNode& rhs);

/// @class ExchangeNodeGroup
///
/// @brief A ExchangeNodeGroup is a collection of ExchangeNodes, and is the
/// ExchangeGraph representation of a BidPortfolio or RequestPortfolio. It
/// houses information about the concrete capacities associated with either
/// portfolio.
class ExchangeNodeGroup {
 public:
  typedef boost::shared_ptr<ExchangeNodeGroup> Ptr;
  
  const std::vector<ExchangeNode::Ptr>& nodes() { return nodes_; }

  /// @brief the flow capacities assocaited with this group
  const std::vector<double>& capacities() const { return capacities_; }
  std::vector<double>& capacities() { return capacities_; }

  /// @brief Add the node to the ExchangeNodeGroup and informs the node it is a
  /// member of this ExchangeNodeGroup
  void AddExchangeNode(ExchangeNode::Ptr node);

  /// @brief Add a flow capacity to the group
  inline void AddCapacity(double c) { capacities_.push_back(c); }

 private:
  std::vector<ExchangeNode::Ptr> nodes_;
  std::vector<double> capacities_;
};

/// @class RequestGroup
///
/// @brief A RequestGroup is a specific ExchangeNodeGroup with a notion of an total
/// requested quantity.
class RequestGroup : public ExchangeNodeGroup {
 public:
  typedef boost::shared_ptr<RequestGroup> Ptr;
  
  explicit RequestGroup(double qty = 0.0);

  double qty() { return qty_; }
  
 private:
  double qty_;
};

/// @brief the capacity of the arc
///
/// @throws StateError if either ExchangeNode does not have a ExchangeNodeGroup
/// @param a the arc
/// @return The minimum of the unode and vnode's capacities
double Capacity(const Arc& a);

/// @brief the capacity of a node
///
/// @throws StateError if ExchangeNode does not have a ExchangeNodeGroup
/// @param n the node
/// @return The minimum of the node's nodegroup capacities / the node's unit
/// capacities, or the ExchangeNode's remaining qty -- whichever is smaller. 
double Capacity(ExchangeNode& n, const Arc& a);
double Capacity(ExchangeNode::Ptr pn, const Arc& a);

/// @brief updates the capacity of a given ExchangeNode (i.e., its max_qty and the
/// capacities of its ExchangeNodeGroup)
///
/// @throws StateError if ExchangeNode does not have a ExchangeNodeGroup
/// @throws ValueError if the update results in a negative ExchangeNodeGroup
/// capacity or a negative ExchangeNode max_qty
/// @param n the ExchangeNode
/// @param qty the quantity for the node to update
void UpdateCapacity(ExchangeNode& n, const Arc& a, double qty);
void UpdateCapacity(ExchangeNode::Ptr pn, const Arc& a, double qty);

typedef std::pair<Arc, double> Match;

/// @class ExchangeGraph
///
/// @brief An ExchangeGraph is a resource-neutral representation of a
/// ResourceExchange. In general, it is produced via translation by an
/// ExchangeTranslator. It is comprised of ExchangeNodes that are collected into
/// ExchangeNodeGroups. Arcs are defined, connecting ExchangeNodes to each
/// other. An ExchangeSolver can solve a given instance of an ExchangeGraph, and
/// the solution is stored on the Graph in the form of Matches.
class ExchangeGraph {
 public: 
  typedef boost::shared_ptr<ExchangeGraph> Ptr;

  /// @brief adds a request group to the graph
  void AddRequestGroup(RequestGroup::Ptr prs);
  
  /// @brief adds a supply group to the graph
  void AddSupplyGroup(ExchangeNodeGroup::Ptr prs);
  
  /// @brief adds an arc to the graph
  void AddArc(const Arc& a);

  /// @brief adds a match for a quanity of flow along an arc
  ///
  /// @param pa the arc corresponding to a match
  /// @param qty the amount of flow corresponding to a match
  void AddMatch(const Arc& a, double qty);

  inline const std::vector<RequestGroup::Ptr>& request_groups() {
    return request_groups_;
  }
  
  inline const std::vector<ExchangeNodeGroup::Ptr>& supply_groups() {
    return supply_groups_;
  }
  
  inline const std::map<ExchangeNode::Ptr, std::vector<Arc> >& node_arc_map() {
    return node_arc_map_;
  }
  
  inline const std::vector<Match>& matches() { return matches_; }
  
  inline const std::vector<Arc>& arcs() { return arcs_; }
  
 private: 
  std::vector<RequestGroup::Ptr> request_groups_;
  std::vector<ExchangeNodeGroup::Ptr> supply_groups_;
  std::map<ExchangeNode::Ptr, std::vector<Arc> > node_arc_map_;
  std::vector<Match> matches_;
  std::vector<Arc> arcs_;
};
  
} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_GRAPH_H_
