#ifndef CYCLUS_SRC_COMMODITY_PRODUCER_MANAGER_H_
#define CYCLUS_SRC_COMMODITY_PRODUCER_MANAGER_H_

#include <set>

#include "commodity.h"
#include "commodity_producer.h"

namespace cyclus {

/// a mixin to provide information about commodity producers
class CommodityProducerManager {
 public:
  /// constructor
  CommodityProducerManager();

  /// virtual destructor for inheritance
  virtual ~CommodityProducerManager();

  /// @return the total production capacity for a commodity amongst producers
  /// @param commodity the commodity in question
  double TotalProductionCapacity(Commodity& commodity);

  // protected: @MJGFlag - should be protected. Revise when tests can
  // be found by classes in the Utility folder
  /// register a commodity producer with the manager
  /// @param producer the producer
  void RegisterProducer(CommodityProducer* producer);

  /// unregister a commodity producer with the manager
  /// @param producer the producer
  void UnRegisterProducer(CommodityProducer* producer);

  /// the set of managed producers
  std::set<CommodityProducer*> producers_;

  // #include "commodity_producer_manager_tests.h"
  // friend class CommodityProducerManagerTests;
  // @MJGFlag - removed for the same reason as above
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_COMMODITY_PRODUCER_MANAGER_H_
