#ifndef CYCLUS_EXCHANGE_TRANSLATOR_H_
#define CYCLUS_EXCHANGE_TRANSLATOR_H_

#include "exchange_graph.h"
#include "request.h"
#include "request_portfolio.h"
#include "bid.h"
#include "bid_portfolio.h"
#include "trade.h"

namespace cyclus {

template <class T> class ExchangeContext;
class Trader;

/// @class ExchangeTranslator
///
/// @brief An ExchangeTranslator facilitates translation from a resource
/// specific exchange to a resource-neutral exchange, upon which generic solvers
/// can be applied. Specifically, the ExchangeTranslator constructor takes a
/// resource-specific exchange context and provides a Translate() method to make
/// a resource-neutral ExchangeGraph. State is maintained in the
/// ExchangeTranslator class, mapping Requests and Bids to Nodes in the
/// ExchangeGraph. Accordingly, the solution to the ExchangeGraph, i.e., it's
/// Matches, can be back-translated to the original Requests and Bids via a
/// BackTranslateSolution() method.
template <class T>
class ExchangeTranslator {
 public:
  /// @brief default constructor
  ///
  /// @param ctx the exchance context
  ExchangeTranslator(ExchangeContext<T>* ctx) {
    ctx_ = ctx;    
  };

  /// @brief translate the ExchangeContext into an ExchangeGraph
  ExchangeGraph::Ptr Translate() {
    ExchangeGraph::Ptr graph(new ExchangeGraph());

    const std::vector< BidPortfolio<T> >& bids = ctx_->bids();
    typename std::vector< BidPortfolio<T> >::const_iterator b_it;
    for (b_it = bids.begin(); b_it != bids.end(); ++b_it) {
      NodeSet::Ptr ns = __TranslateBidPortfolio(*b_it);
      graph->AddSupplySet(ns);
    }
    
    return graph;
  };
  
  /// @brief Provide a vector of Trades given a vector of Matches
  std::vector< Trade<T> >
      BackTranslateSolution(const std::vector<Match>& matches) {
    std::vector< Trade<T> > ret;
    std::vector<Match>::const_iterator m_it;
    for (m_it = matches.begin(); m_it != matches.end(); ++m_it) {
      ret.push_back(__BackTranslateMatch(*m_it));
    }
    return ret;
  };
  
  /* -------------------- private methods and members -------------------------- */
  ExchangeContext<T>* ctx_;
  std::map<typename Request<T>::Ptr, Node::Ptr> request_to_node_;
  std::map<Node::Ptr, typename Request<T>::Ptr> node_to_request_;
  std::map<typename Bid<T>::Ptr, Node::Ptr> bid_to_node_;
  std::map<Node::Ptr, typename Bid<T>::Ptr> node_to_bid_;
  
  /// @brief translates a bid portfolio by translating each bid with
  /// __TranslateBid() and adding capacity values
  NodeSet::Ptr __TranslateBidPortfolio(BidPortfolio<T>& bp) {
    NodeSet::Ptr ns(new NodeSet());
    
    typename std::set<typename Bid<T>::Ptr>::const_iterator b_it;
    for (b_it = bp.bids().begin(); b_it != bp.bids.end(); ++b_it) {
      Node::Ptr n(new Node());
      ns->AddNode(n);
      __AddBid(*b_it, n); // unit capacities must be calculated when arc is known
    }
    
    typename std::set< CapacityConstraint<T> >::const_iterator c_it;
    for (c_it = bp.constraints.begin(); c_it != bp.constraints.end(); ++c_it) {
      ns->capacities.push_back(c_it->capacity);
    }
    
    return ns;
  }

  /// /// @brief translates an individual bid
  /// Node::Ptr __TranslateBid(
  ///     typename Bid<T>::Ptr b,
  ///     const typename std::set< CapacityConstraint<T> >& constraints) {
  ///   Node::Ptr n;
    
  ///   typename std::set< CapacityConstraint<T> >::const_iterator it;
  ///   for (it = constraints.begin(); it != constraints.end(); ++it) {
  ///     n->unit_capacities.push_back(it->converter(b->offer) / it->capacity);
  ///   }
    
  ///   return n;
  /// }

  /// @brief Adds a request-node mapping
  void __AddRequest(typename Request<T>::Ptr r, Node::Ptr n) {
    request_to_node_[r] = n;
    node_to_request_[n] = r;
  }

  /// @brief Adds a bid-node mapping
  void __AddBid(typename Bid<T>::Ptr b, Node::Ptr n) {
    bid_to_node_[b] = n;
    node_to_bid_[n] = b;
  }
  
  /// @brief simple translation from a Match to a Trade, given internal state
  Trade<T> __BackTranslateMatch(const Match& match) {
    Node::Ptr req_node = match.first->unode;
    Node::Ptr bid_node = match.first->vnode;
    
    Trade<T> t;
    t.request = node_to_request_[req_node];
    t.bid = node_to_bid_[bid_node];
    t.amt = match.second;
    return t;
  };
};

} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_TRANSLATOR_H_
