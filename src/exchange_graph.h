#ifndef CYCLUS_SRC_EXCHANGE_GRAPH_H_
#define CYCLUS_SRC_EXCHANGE_GRAPH_H_

#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "capacity_types.h"

namespace cyclus {

class ExchangeNodeGroup;
class Arc;

/// @class ExchangeNode
///
/// @brief ExchangeNodes are used in ExchangeGraphs to house information about a
/// given translated Bid or Request. Specifically, ExchangeNodes have a notion
/// of unit capacities that the given Bid or Request contribute to the overall
/// capacity of ExchangeNodeGroup. ExchangeNodes also have a notion of quantity,
/// i.e., the maximum amount of a resource that can be attributed to
/// it. Finally, nodes can be exclusive, that is to say that they represent a
/// request or bid that must be exclusively satisfied (it can not be split).
struct ExchangeNode {
 public:
  typedef boost::shared_ptr<ExchangeNode> Ptr;

  ExchangeNode();
  explicit ExchangeNode(double qty);
  ExchangeNode(double qty, bool exclusive);
  ExchangeNode(double qty, bool exclusive, std::string commod);
  ExchangeNode(double qty, bool exclusive, std::string commod, int agent_id);

  /// @brief the parent ExchangeNodeGroup to which this ExchangeNode belongs
  ExchangeNodeGroup* group;

  /// @brief unit values associated with this ExchangeNode corresponding to
  /// capacties of its parent ExchangeNodeGroup. This information corresponds to
  /// the resource object from which this ExchangeNode was translated.
  std::map<Arc, std::vector<double> > unit_capacities;

  /// @brief preference values for arcs
  std::map<Arc, double> prefs;

  /// @brief whether this node represents an exclusive request or offer
  bool exclusive;

  /// @brief the commodity associated with this exchange node
  std::string commod;

  /// @brief the id of the agent associated with this node
  int agent_id;

  /// @brief the maximum amount of a resource that can be associated with this
  /// node
  double qty;
};

/// @brief An arc represents a possible connection between two nodes in the
/// bipartite resource exchange graph. It is common to refer to the two sets of
/// nodes in a bipartite graph by the set variables U and V (see
/// http://en.wikipedia.org/wiki/Bipartite_graph). By convention, arc.unode() ==
/// request node, arc.vnode() == bid node.
class Arc {
 public:
  /// default required for usage in maps
  /// @warning, in general do not use this constructor; it exists for arcs to be
  /// map values
  Arc() {}

  Arc(boost::shared_ptr<ExchangeNode> unode,
      boost::shared_ptr<ExchangeNode> vnode);

  Arc(const Arc& other);

  inline Arc& operator=(const Arc& other) {
    unode_ = other.unode();
    vnode_ = other.vnode();
    exclusive_ = other.exclusive();
    excl_val_ = other.excl_val();
    return *this;
  }

  inline bool operator <(const Arc& rhs) const {
    return unode_ < rhs.unode_ ||
        (!(rhs.unode_ < unode_) && vnode_ < rhs.vnode_);
  }

  inline bool operator==(const Arc& rhs) const {
    return unode() == rhs.unode() && vnode() == rhs.vnode();
  }

  inline boost::shared_ptr<ExchangeNode> unode() const { return unode_.lock(); }
  inline boost::shared_ptr<ExchangeNode> vnode() const { return vnode_.lock(); }
  inline bool exclusive() const { return exclusive_; }
  inline double excl_val() const { return excl_val_; }

 private:
  boost::weak_ptr<ExchangeNode> unode_;
  boost::weak_ptr<ExchangeNode> vnode_;
  bool exclusive_;
  double excl_val_;
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

  const std::vector<ExchangeNode::Ptr>& nodes() const { return nodes_; }
  std::vector<ExchangeNode::Ptr>& nodes() { return nodes_; }

  /// @brief exclusive node groups represent nodes over whose combined arcs flow
  /// can only exist on one arc
  const std::vector< std::vector<ExchangeNode::Ptr> >&
      excl_node_groups() const {
    return excl_node_groups_;
  }
  std::vector< std::vector<ExchangeNode::Ptr> >& excl_node_groups() {
    return excl_node_groups_;
  }

  /// @brief the flow capacities assocaited with this group
  const std::vector<double>& capacities() const { return capacities_; }
  std::vector<double>& capacities() { return capacities_; }

  /// @brief the kind of capacities (i.e., LTEQ, GTEQ, EQ) 
  const std::vector<cap_t>& cap_types() const { return cap_types_; }
  std::vector<cap_t>& cap_types() { return cap_types_; }

  /// @brief Add the node to the ExchangeNodeGroup and informs the node it is a
  /// member of this ExchangeNodeGroup
  virtual void AddExchangeNode(ExchangeNode::Ptr node);

  /// @brief Adds a node grouping to the set of exclusive node groups, in
  /// general this function is used for bid exclusivity. An exclusive group
  /// implies that for all nodes in that group, flow is only allowed to flow
  /// over one. This is the case for multiple bids that refer to the same
  /// exclusive object.
  inline void AddExclGroup(std::vector<ExchangeNode::Ptr>& nodes) {
    excl_node_groups_.push_back(nodes);
  }

  /// @brief adds a single node to the set of exclusive node groupings, in
  /// general this function is used for demand exclusivity
  void AddExclNode(ExchangeNode::Ptr n);

  /// @brief Add a flow capacity to the group
  /// @param c the rhs for the capacity
  /// @param t the capacity type, the default for exchange groups is LTEQ
  // @{
  inline virtual void AddCapacity(double c) {
    capacities_.push_back(c);
    cap_types_.push_back(LTEQ);
  }
  inline void AddCapacity(double c, cap_t t) {
    capacities_.push_back(c);
    cap_types_.push_back(t);
  }
  // @}

 protected:
  std::vector<double> capacities_;
  std::vector<cap_t> cap_types_;
  
 private:
  std::vector<ExchangeNode::Ptr> nodes_;
  std::vector< std::vector<ExchangeNode::Ptr> > excl_node_groups_;
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

  /// @brief Add the node to the ExchangeNodeGroup and informs the node it is a
  /// member of this ExchangeNodeGroup, if the node is exclusive, also add it to
  /// the group of exclusive nodes
  virtual void AddExchangeNode(ExchangeNode::Ptr node);

  /// @brief Add a flow capacity to the group
  /// @param c the rhs for the capacity
  /// @param t the capacity type, the default for request groups is GTEQ
  // @{
  inline virtual void AddCapacity(double c) {
    capacities_.push_back(c);
    cap_types_.push_back(GTEQ);
  }
  // @}
  
 private:
  double qty_;
};

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

  ExchangeGraph();

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

  /// clears all matches
  inline void ClearMatches() { matches_.clear(); }

  inline const std::vector<RequestGroup::Ptr>& request_groups() const {
    return request_groups_;
  }
  inline std::vector<RequestGroup::Ptr>& request_groups() {
    return request_groups_;
  }

  inline const std::vector<ExchangeNodeGroup::Ptr>& supply_groups() const {
    return supply_groups_;
  }
  inline std::vector<ExchangeNodeGroup::Ptr>& supply_groups() {
    return supply_groups_;
  }

  inline const std::map<ExchangeNode::Ptr, std::vector<Arc> >&
      node_arc_map() const {
    return node_arc_map_;
  }
  inline std::map<ExchangeNode::Ptr, std::vector<Arc> >& node_arc_map() {
    return node_arc_map_;
  }

  inline const std::vector<Match>& matches() { return matches_; }

  inline const std::vector<Arc>& arcs() const { return arcs_; }
  inline std::vector<Arc>& arcs() { return arcs_; }

  inline const std::map<Arc, int>& arc_ids() const { return arc_ids_; }
  inline std::map<Arc, int>& arc_ids() { return arc_ids_; }

  inline const std::map<int, Arc>& arc_by_id() const { return arc_by_id_; }
  inline std::map<int, Arc>& arc_by_id() { return arc_by_id_; }
  
 private:
  std::vector<RequestGroup::Ptr> request_groups_;
  std::vector<ExchangeNodeGroup::Ptr> supply_groups_;
  std::map<ExchangeNode::Ptr, std::vector<Arc> > node_arc_map_;
  std::vector<Match> matches_;
  std::vector<Arc> arcs_;
  std::map<Arc, int> arc_ids_;
  std::map<int, Arc> arc_by_id_;
  int next_arc_id_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_EXCHANGE_GRAPH_H_
