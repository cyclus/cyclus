#ifndef CYCLUS_SRC_TOOLKIT_BUILDER_H_
#define CYCLUS_SRC_TOOLKIT_BUILDER_H_

#include <set>

#include "agent_managed.h"
#include "commodity_producer.h"

namespace cyclus {
namespace toolkit {

/// a mixin to provide information about commodity producers that can
/// be built
class Builder: public AgentManaged {
 public:
  Builder(Agent* agent=NULL) : AgentManaged(agent) {};
  virtual ~Builder() {};

  /// register a commodity producer with the agent
  /// @param producer the producer
  inline void Register(CommodityProducer* producer) {
    producers_.insert(producer);
  }

  /// unregister a commodity producer with the agent
  /// @param producer the producer
  inline void Unregister(CommodityProducer* producer) {
    producers_.erase(producer);
  }

  inline const std::set<CommodityProducer*>& producers() const {return producers_;}
  
 private:
  std::set<CommodityProducer*> producers_;
};

} // namespace toolkit
} // namespace cyclus
#endif  // CYCLUS_SRC_TOOLKIT_BUILDER_H_
