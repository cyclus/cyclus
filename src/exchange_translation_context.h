#ifndef CYCLUS_EXCHANGE_TRANSLATION_CONTEXT_H_
#define CYCLUS_EXCHANGE_TRANSLATION_CONTEXT_H_

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
  std::map<typename Request<T>::Ptr, ExchangeNode::Ptr> request_to_node;
  std::map<ExchangeNode::Ptr, typename Request<T>::Ptr> node_to_request;
  std::map<typename Bid<T>::Ptr, ExchangeNode::Ptr> bid_to_node;
  std::map<ExchangeNode::Ptr, typename Bid<T>::Ptr> node_to_bid;
};

} // namespace cyclus

#endif // CYCLUS_EXCHANGE_TRANSLATION_CONTEXT_H_
