#ifndef CYCLUS_SRC_BUILDER_H_
#define CYCLUS_SRC_BUILDER_H_

#include <set>

#include "agent.h"
#include "commodity_producer.h"

namespace cyclus {
namespace toolkit {

/// a mixin to provide information about commodity producers that can
/// be built
class Builder {
 public:
  Builder(Agent* manager=NULL) : manager_(manager) {};
  virtual ~Builder() {};

  /// register a commodity producer with the manager
  /// @param producer the producer
  inline void Register(CommodityProducer* producer) {
    producers_.insert(producer);
  }

  /// unregister a commodity producer with the manager
  /// @param producer the producer
  inline void Unregister(CommodityProducer* producer) {
    producers_.erase(producer);
  }

  const std::set<CommodityProducer*>& producers() const {return producers_;}

  Agent* manager() const {return manager_;}
  
 protected:
  Agent* manager_;
  std::set<CommodityProducer*> producers_;
};

} // namespace toolkit
} // namespace cyclus
#endif  // CYCLUS_SRC_BUILDER_H_
