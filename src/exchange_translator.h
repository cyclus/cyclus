#ifndef CYCLUS_EXCHANGE_TRANSLATOR_H_
#define CYCLUS_EXCHANGE_TRANSLATOR_H_

#include "exchange_graph.h"
#include "request.h"
#include "bid.h"
#include "trade.h"

namespace cyclus {

template <class T> class ExchangeContext;
class Trader;

template <class T>
class ExchangeTranslator {
 public:
  /// @brief default constructor
  ///
  /// @param ctx the exchance context
  ExchangeTranslator(ExchangeContext<T>* ctx) {
    ctx_ = ctx;    
  };

  ExchangeGraph::Ptr Translate();

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

  Trade<T> __BackTranslateMatch(const Match& match) {
    Trade<T> t;
    Node::Ptr req_node = match.first->unode;
    typename Request<T>::Ptr req = node_to_request_[req_node];
    Node::Ptr bid_node = match.first->vnode;
    typename Bid<T>::Ptr bid = node_to_bid_[bid_node];

    t.sender = bid.bidder;
    t.reciver = req.requester;
    // this is suspect... I'm not sure if this is the right thing to do in our
    // system
    t.item = bid.offer->ExtractRes(match.second);
    return t;
  };
};

} // namespace cyclus

#endif // ifndef CYCLUS_EXCHANGE_TRANSLATOR_H_
