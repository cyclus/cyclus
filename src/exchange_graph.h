#ifndef CYCLUS_EXCHANGE_GRAPH_H_
#define CYCLUS_EXCHANGE_GRAPH_H_

#include <map>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

namespace cyclus {

class NodeSet;
class Arc;

class Node {
 public:
  typedef boost::shared_ptr<Node> Ptr;

  Node();
  
  std::vector<double> unit_capacities;

  NodeSet* set;
};

class NodeSet {
 public:
  typedef boost::shared_ptr<NodeSet> Ptr;

  std::vector<Node::Ptr> nodes;
  std::vector<double> capacities;

  /// @brief Add the node to the NodeSet and informs the node it is a member of
  /// this NodeSet
  void AddNode(Node::Ptr node);
};

class RequestSet : public NodeSet {
 public:
  typedef boost::shared_ptr<RequestSet> Ptr;
  
  explicit RequestSet(double qty = 0.0);
  double qty;
};

struct Arc {
 public:
  typedef boost::shared_ptr<Arc> Ptr;

  Arc(Node::Ptr unode, Node::Ptr vnode);
  
  Node::Ptr unode;
  Node::Ptr vnode;
};

/// @brief the capacity of the arc
///
/// @param a the arc
/// @return The minimum of the unode and vnode's capacities
double Capacity(const Arc& a);
double Capacity(Arc::Ptr pa);

/// @brief the capacity of a node
///
/// @param n the node
/// @return The minimum of the node's nodeset capacities / the node's unit
/// capacities. If the node/node set have no capacities, std::max<double> is
/// returned.
double Capacity(const Node& n);
double Capacity(Node::Ptr pn);

/// @brief updates the capacity of a given node
///
/// @param n the Node
/// @param qty the quantity for the node to update
void UpdateCapacity(const Node& n, double qty);
void UpdateCapacity(Node::Ptr pn, double qty);

typedef std::pair<Arc::Ptr, double> Match;

class ExchangeGraph {
 public: 
  typedef boost::shared_ptr<ExchangeGraph> Ptr;

  std::vector<RequestSet::Ptr> request_sets;
  std::vector<NodeSet::Ptr> supply_sets;
  std::map<Node::Ptr, std::vector<Arc::Ptr> > node_arc_map;
  std::vector<Match> matches;

  /// @brief use the AddArc() api to update arcs_
  std::vector<Arc::Ptr> arcs_;

  /// @brief adds a request set to the graph
  void AddRequestSet(RequestSet::Ptr prs);
  
  /// @brief adds a supply set to the graph
  void AddSupplySet(NodeSet::Ptr prs);
  
  /// @brief adds an arc to the graph
  void AddArc(Arc::Ptr pa);

  /// @brief adds a match for a quanity of flow along an arc
  ///
  /// @param pa the arc corresponding to a match
  /// @param qty the amount of flow corresponding to a match
  void AddMatch(Arc::Ptr pa, double qty);
};
  
} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_GRAPH_H_
