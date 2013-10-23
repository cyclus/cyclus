#ifndef BUILDER_H
#define BUILDER_H

#include <set>

#include "commodity_producer.h"

namespace cyclus {

/// a mixin to provide information about commodity producers that can
/// be built
class Builder {
 public:
  /// constructor
  Builder();

  /// virtual destructor for inheritence
  virtual ~Builder();

  // protected: @MJGFlag - should be protected. revise when tests can
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

 private:
  /// the set of managed producers
  std::set<CommodityProducer*> producers_;

  //#include "commodity_producer_manager_tests.h"
  //friend class CommodityProducerManagerTests;
  // @MJGFlag - removed for the same reason as above
};

} // namespace cyclus
#endif
