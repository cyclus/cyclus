#ifndef CYCLUS_SRC_BUILDER_H_
#define CYCLUS_SRC_BUILDER_H_

#include <set>

#include "commodity_producer.h"

namespace cyclus {
namespace toolkit {

/// a mixin to provide information about commodity producers that can
/// be built
class Builder {
 public:
  /// constructor
  Builder();

  /// virtual destructor for inheritance
  virtual ~Builder();

  // protected: @MJGFlag - should be protected. Revise when tests can
  // be found by classes in the Utility folder
  /// register a commodity producer with the manager
  /// @param producer the producer
  void RegisterProducer(CommodityProducer* producer);

  /// unregister a commodity producer with the manager
  /// @param producer the producer
  void UnRegisterProducer(CommodityProducer* producer);

  /// @return the number of buildings managed by this builder
  double NBuildingPrototypes();

  /// @return the beginning iterator of the set of producers
  std::set<CommodityProducer*>::iterator BeginningProducer();

  /// @return the beginning iterator of the set of producers
  std::set<CommodityProducer*>::iterator EndingProducer();

 protected:
  /// the set of managed producers
  std::set<CommodityProducer*> commod_producers_;

  // #include "commodity_producer_manager_tests.h"
  // friend class CommodityProducerManagerTests;
  // @MJGFlag - removed for the same reason as above
};

} // namespace toolkit
} // namespace cyclus
#endif  // CYCLUS_SRC_BUILDER_H_
