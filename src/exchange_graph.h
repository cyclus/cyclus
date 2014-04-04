#ifndef CYCLUS_EXCHANGE_GRAPH_H_
#define CYCLUS_EXCHANGE_GRAPH_H_

#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

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
  
  ExchangeNode(double qty = std::numeric_limits<double>::max(),
               bool exclusive = false,
               std::string commod = "");

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
  Arc() {};

  Arc(boost::shared_ptr<ExchangeNode> unode,
      boost::shared_ptr<ExchangeNode> vnode);

  Arc(const Arc& other);

  inline Arc& operator=(const Arc& other) {
    unode_ = other.unode();
    vnode_ = other.vnode();
    exclusive_ = other.exclusive();
    excl_val_ = other.excl_val();
    return *this;
  };

  inline bool operator <(const Arc& rhs) const {
    return unode_ < rhs.unode() ||
        (!(rhs.unode() < unode_) && vnode_ < rhs.vnode());
  }

  inline bool operator==(const Arc& rhs) const {
    return unode_ == rhs.unode() && vnode_ == rhs.vnode();
  };
    
  inline boost::shared_ptr<ExchangeNode> unode() const { return unode_; }
  inline boost::shared_ptr<ExchangeNode> vnode() const { return vnode_; }  
  inline bool exclusive() const { return exclusive_; }
  inline double excl_val() const { return excl_val_; }

 private:
  boost::shared_ptr<ExchangeNode> unode_;
  boost::shared_ptr<ExchangeNode> vnode_;
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

  /// @brief exclusive node groups represent nodes over which flow can only
  /// exist on one arc
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

  /// @brief Add the node to the ExchangeNodeGroup and informs the node it is a
  /// member of this ExchangeNodeGroup
  virtual void AddExchangeNode(ExchangeNode::Ptr node);

  /// @brief Adds a node grouping to the set of exclusive node groups, in
  /// general this function is used for request exclusivity
  inline void AddExclGroup(std::vector<ExchangeNode::Ptr>& nodes) {
    excl_node_groups_.push_back(nodes);
  }

  /// @brief adds a single node to the set of exclusive node groupings, in
  /// general this function is used for demand exclusivity
  void AddExclNode(ExchangeNode::Ptr n);

  /// @brief Add a flow capacity to the group
  inline void AddCapacity(double c) { capacities_.push_back(c); }

 private:
  std::vector<ExchangeNode::Ptr> nodes_;
  std::vector< std::vector<ExchangeNode::Ptr> > excl_node_groups_;
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

  /// @brief Add the node to the ExchangeNodeGroup and informs the node it is a
  /// member of this ExchangeNodeGroup, if the node is exclusive, also add it to
  /// the group of exclusive nodes
  virtual void AddExchangeNode(ExchangeNode::Ptr node);
  
 private:
  double qty_;
};

/// @brief the capacity of the arc
///
/// @throws StateError if either ExchangeNode does not have a ExchangeNodeGroup
/// @param a the arc
/// @param u_curr_qty the current quantity assigned to the unode (if solving
/// piecemeal)
/// @param v_curr_qty the current quantity assigned to the vnode (if solving
/// piecemeal)
/// @return The minimum of the unode and vnode's capacities
double Capacity(const Arc& a, double u_curr_qty = 0, double v_curr_qty = 0);

/// @brief the capacity of a node
///
/// @throws StateError if ExchangeNode does not have a ExchangeNodeGroup
/// @param n the node
/// @param curr_qty the currently allocated node quantity (if solving piecemeal)
/// @return The minimum of the node's nodegroup capacities / the node's unit
/// capacities, or the ExchangeNode's remaining qty -- whichever is smaller. 
double Capacity(ExchangeNode::Ptr n, const Arc& a, double curr_qty = 0.0);

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
  
} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_GRAPH_H_
