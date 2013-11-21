#ifndef CYCLUS_EXCHANGE_GRAPH_H_
#define CYCLUS_EXCHANGE_GRAPH_H_

#include <map>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

namespace cyclus {

class NodeSet;
class Node;

/// @brief by convention, arc.first == request node (unode), arc.second == bid
/// node (vnode)
typedef std::pair< boost::shared_ptr<Node>, boost::shared_ptr<Node> > Arc;

/// @brief Nodes are used in ExchangeGraphs to house information about a given
/// translated Bid or Request. Specifically, Nodes have a notion of unit
/// capacities that the given Bid or Request contribute to the overall capacity
/// of NodeSet.
struct Node {
 public:
  typedef boost::shared_ptr<Node> Ptr;
  
  Node();
  
  std::map<Arc, std::vector<double> > unit_capacities;

  NodeSet* set;
};

bool operator==(const Node& lhs, const Node& rhs);

/// @class NodeSet
///
/// @brief A NodeSet is a collection of Nodes, and is the ExchangeGraph
/// representation of a BidPortfolio or RequestPortfolio. It houses information
/// about the concrete capacities associated with either portfolio.
class NodeSet {
 public:
  typedef boost::shared_ptr<NodeSet> Ptr;
  
  std::vector<Node::Ptr> nodes;
  std::vector<double> capacities;

  /// @brief Add the node to the NodeSet and informs the node it is a member of
  /// this NodeSet
  void AddNode(Node::Ptr node);
};

/// @class RequestSet
///
/// @brief A RequestSet is a specific NodeSet with a notion of an total
/// requested quantity.
class RequestSet : public NodeSet {
 public:
  typedef boost::shared_ptr<RequestSet> Ptr;
  
  explicit RequestSet(double qty = 0.0);
  double qty;
};

/// @brief the capacity of the arc
///
/// @param a the arc
/// @return The minimum of the unode and vnode's capacities
double Capacity(const Arc& a);

/// @brief the capacity of a node
///
/// @param n the node
/// @return The minimum of the node's nodeset capacities / the node's unit
/// capacities. If the node/node set have no capacities, std::max<double> is
/// returned.
double Capacity(Node& n, const Arc& a);
double Capacity(Node::Ptr pn, const Arc& a);

/// @brief updates the capacity of a given node
///
/// @param n the Node
/// @param qty the quantity for the node to update
void UpdateCapacity(Node& n, const Arc& a, double qty);
void UpdateCapacity(Node::Ptr pn, const Arc& a, double qty);

typedef std::pair<Arc, double> Match;

/// @class ExchangeGraph
///
/// @brief An ExchangeGraph is a resource-neutral representation of a
/// ResourceExchange. In general, it is produced via translation by an
/// ExchangeTranslator. It is comprised of Nodes that are collected into
/// NodeSets. Arcs are defined, connecting Nodes to each other. An
/// ExchangeSolver can solve a given instance of an ExchangeGraph, and the
/// solution is stored on the Graph in the form of Matches.
class ExchangeGraph {
 public: 
  typedef boost::shared_ptr<ExchangeGraph> Ptr;

  std::vector<RequestSet::Ptr> request_sets;
  std::vector<NodeSet::Ptr> supply_sets;
  std::map<Node::Ptr, std::vector<Arc> > node_arc_map;
  std::vector<Match> matches;

  /// @brief use the AddArc() api to update arcs_
  std::vector<Arc> arcs_;

  /// @brief adds a request set to the graph
  void AddRequestSet(RequestSet::Ptr prs);
  
  /// @brief adds a supply set to the graph
  void AddSupplySet(NodeSet::Ptr prs);
  
  /// @brief adds an arc to the graph
  void AddArc(const Arc& a);

  /// @brief adds a match for a quanity of flow along an arc
  ///
  /// @param pa the arc corresponding to a match
  /// @param qty the amount of flow corresponding to a match
  void AddMatch(const Arc& a, double qty);
};
  
} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_GRAPH_H_
