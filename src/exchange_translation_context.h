#ifndef CYCLUS_SRC_EXCHANGE_TRANSLATION_CONTEXT_H_
#define CYCLUS_SRC_EXCHANGE_TRANSLATION_CONTEXT_H_

#include <map>

#include "bid.h"
#include "exchange_graph.h"
#include "request.h"

namespace cyclus {

/// @class ExchangeTranslationContext
///
/// @brief An ExchangeTranslationContext is a simple holder class for any
/// information needed to translate a ResourceExchange to and from an
/// ExchangeGraph
template <class T>
struct ExchangeTranslationContext {
 public:
  std::map<Request<T>*, ExchangeNode::Ptr> request_to_node;
  std::map<ExchangeNode::Ptr, Request<T>*> node_to_request;
  std::map<Bid<T>*, ExchangeNode::Ptr> bid_to_node;
  std::map<ExchangeNode::Ptr, Bid<T>*> node_to_bid;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_EXCHANGE_TRANSLATION_CONTEXT_H_
