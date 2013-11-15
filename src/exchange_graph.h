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
/// determined by the minimum of the unode and vnode's capacities / unit
/// capacities
///
/// @param a the arc
double Capacity(const Arc& a);

class ExchangeGraph {

  
};
  
} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_GRAPH_H_
