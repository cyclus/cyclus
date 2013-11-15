#ifndef CYCLUS_EXCHANGE_GRAPH_H_
#define CYCLUS_EXCHANGE_GRAPH_H_

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
  std::vector<Node::Ptr> nodes;
  std::vector<double> capacities;

  /// @brief Add the node to the NodeSet and informs the node it is a member of
  /// this NodeSet
  void AddNode(Node::Ptr node);
  
  /// @brief updates capacities for a given node
  ///
  /// for each capacity, removes the qty * node->unit_capacity
  /// @param node the updated node
  /// @param qty the quantity 
  void UpdateCapacities(Node::Ptr node, double qty);
};

class RequestSet : public NodeSet {
 public:
  explicit RequestSet(double qty = 0.0);
  double qty;
};

struct Arc {
 public:
  Node::Ptr unode;
  Node::Ptr vnode;
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
double Capacity(const Node& n);

/// @brief overload of Capacity() for Node::Ptr
double Capacity(const Node::Ptr& pn);

class ExchangeGraph {

  
};
  
} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_GRAPH_H_
