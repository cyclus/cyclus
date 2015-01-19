#ifndef CYCLUS_SRC_LUSTY_SOLVER_H_
#define CYCLUS_SRC_LUSTY_SOLVER_H_

#include "exchange_graph.h"
#include "exchange_solver.h"
#include "greedy_preconditioner.h"

namespace cyclus {

/// \brief A comparison function for sorting a container of Arcs by the
/// supplier's (vnode's) preference, in decensing order (i.e., most preferred
/// Arc first). In the case of a tie, the node ids are used.
inline bool SupQuantityCompare(const Arc& l, const Arc& r) {
  int lu = l.unode()->agent_id;
  int lv = l.vnode()->agent_id;
  int ru = r.unode()->agent_id;
  int rv = r.vnode()->agent_id;
  double lqty = l.vnode()->qty;
  double rqty = r.vnode()->qty;
  return (lqty != rqty) ? (lqty > rqty) : (lu > ru || (lu == ru && lv > rv));
}

/// \brief A comparison function for sorting a container of Nodes by the node's
/// quantities in decensing order (i.e., most preferred Node first). In the
/// case of a tie, the node ids are used.
inline bool QuantityCompare(ExchangeNode::Ptr l, ExchangeNode::Ptr r) {
  int lid = l->agent_id;
  int rid = r->agent_id;
  double lqty = l->qty;
  double rqty = r->qty;
  return (lqty != rqty) ? (lqty > rqty) : (lid > rid);
}

class ExchangeGraph;
class GreedyPreconditioner;

/// \brief The LustySolver provides the implementation for a "lusty" solution
/// to a resource exchange graph.
///
/// Given an ExchangeGraph, the lusty solver will traverse the SupplyGroup,
/// matching request nodes with supply nodes that have the largest
/// quantity of supply first. This solver terminates in the same way as the
/// greedy solver.
///
/// \warning the LustySolver is responsible for deleting is conditioner!
class LustySolver: public ExchangeSolver {
 public:
  /// LustySolver constructor
  /// \param exclusive_orders a flag for enforcing integral, quantized orders
  /// \param c a conditioner to use before solving a graph instance
  /// \warning if a NULL pointer is passed as a conditioner argument,
  /// conditioning will *NOT* occur
  /// \{
  LustySolver();
  explicit LustySolver(bool exclusive_orders);
  explicit LustySolver(void* c);
  LustySolver(bool exclusive_orders, void* c);
  /// \}

  virtual ~LustySolver();

  /// Uses the provided preconditioner to condition the solver's ExchangeGraph
  /// so that SupplyGroups are ordered by their quantity.
  ///
  /// \warning this function is called during the Solve phase of the DRE and
  /// should most likely not be called otherwise.
  void Condition();

  /// Initialize member values based on the given graph.
  void Init();

  /// \brief the capacity of an arc
  ///
  /// \throws StateError if either ExchangeNode does not have a
  /// ExchangeNodeGroup
  /// \param a the arc
  /// \param u_curr_qty the current quantity assigned to the unode (if solving
  /// piecemeal)
  /// \param v_curr_qty the current quantity assigned to the vnode (if solving
  /// \iecemeal)
  /// \return The minimum of the unode and vnode's capacities
  // \{
  double Capacity(const Arc& a, double u_curr_qty, double v_curr_qty);
  inline double Capacity(const Arc& a) { return Capacity(a, 0, 0); }
  // \}

  /// \brief the capacity of a node
  ///
  /// \throws StateError if ExchangeNode does not have a ExchangeNodeGroup
  /// \param n the node
  /// \param min_cap whether to use the minimum or maximum capacity value.
  /// In general, nodes that represent bids use the minimum (i.e., the
  /// capacities represents a less-than constraint) and nodes that represent
  /// requests use the maximum value (i.e., the capacities represents a
  /// greater-than constraint).
  /// \param curr_qty the currently allocated node quantity (if solving
  /// piecemeal)
  /// \return The minimum of the node's nodegroup capacities / the node's unit
  /// capacities, or the ExchangeNode's remaining qty -- whichever is smaller.
  /// \{
  double Capacity(ExchangeNode::Ptr n, const Arc& a, bool min_cap,
                  double curr_qty);
  inline double Capacity(ExchangeNode::Ptr n, const Arc& a, bool min_cap) {
    return Capacity(n, a, min_cap, 0.0);
  }
  inline double Capacity(ExchangeNode::Ptr n, const Arc& a, double curr_qty) {
    return Capacity(n, a, true, curr_qty);
  }
  inline double Capacity(ExchangeNode::Ptr n, const Arc& a) {
    return Capacity(n, a, true, 0.0);
  }
  /// \}

 protected:
  /// \brief the LustySolver solves an ExchangeGraph by iterating over each
  /// SupplyGroup and matching requests with the minimum bids possible,
  /// starting from the beginning of the the respective request and bid
  /// containers.
  virtual double SolveGraph();

 private:
  /// \brief updates the capacity of a given ExchangeNode (i.e., its max_qty and the
  /// capacities of its ExchangeNodeGroup)
  ///
  /// \throws StateError if ExchangeNode does not have a ExchangeNodeGroup
  /// \throws ValueError if the update results in a negative ExchangeNodeGroup
  /// capacity or a negative ExchangeNode max_qty
  /// \param n the ExchangeNode
  /// \param qty the quantity for the node to update
  void GetCaps(ExchangeNodeGroup::Ptr prs);
  void LustilySatisfySet(RequestGroup::Ptr prs);
  void UpdateCapacity(ExchangeNode::Ptr n, const Arc& a, double qty);
  void UpdateObj(double qty, double pref);

  void* conditioner_;
  std::map<ExchangeNode::Ptr, double> n_qty_;
  std::map<ExchangeNodeGroup*, std::vector<double> > grp_caps_;
  double obj_;
  double unmatched_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_LUSTY_SOLVER_H_
