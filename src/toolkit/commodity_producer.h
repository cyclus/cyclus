#ifndef CYCLUS_SRC_COMMODITY_PRODUCER_H_
#define CYCLUS_SRC_COMMODITY_PRODUCER_H_

#include <map>
#include <set>

#include "commodity.h"

namespace cyclus {
namespace toolkit {

/// a container to hold information about a commodity
struct CommodInfo {
  CommodInfo();
  CommodInfo(double a_capacity, double a_cost);
  double cost;
  double capacity;
};

/// a mixin to provide information about produced commodities
class CommodityProducer {
 public:
  /// constructor
  CommodityProducer();

  /// virtual destructor for inheritance
  virtual ~CommodityProducer();

  /// @return the set of commodities produced by this producers
  std::set<Commodity, CommodityCompare> ProducedCommodities();

  /// @param commodity the commodity in question
  /// @return true if the commodity is produced by this entity
  bool ProducesCommodity(const Commodity& commodity);

  /// @param commodity the commodity in question
  /// @return the production capacity for a commodity
  double ProductionCapacity(const Commodity& commodity);

  /// @return the cost to produce a commodity at a given capacity
  /// @param commodity the commodity in question
  double ProductionCost(const Commodity& commodity);

  // protected: @MJGFlag - should be protected. Revise when tests can
  // be found by classes in the Utility folder
  /// register a commodity as being produced by this object
  /// @param commodity the commodity being produced
  void AddCommodity(const Commodity& commodity);

  /// set the production capacity for a given commodity
  /// @param commodity the commodity being produced
  /// @param capacity the production capacity
  void SetCapacity(const Commodity& commodity, double capacity);

  /// set the production cost for a given commodity
  /// @param commodity the commodity being produced
  /// @param cost the production cost
  void SetCost(const Commodity& commodity, double cost);

  /// register a commodity as being produced by this object and set
  /// its relevant info
  /// @param commodity the commodity being produced
  /// @param info the information describing the commodity
  void AddCommodityWithInformation(const Commodity& commodity,
                                   const CommodInfo& info);

  /// add all commodities produced by a source
  /// @param source the original commodity producer
  void CopyProducedCommoditiesFrom(CommodityProducer* source);

  /// checks if ProducesCommodity() is true. If it is false, an
  /// error is thrown.
  /// @param commodity the commodity in question
  void ThrowErrorIfCommodityNotProduced(const Commodity& commodity);

  /// a default production capacity
  double default_capacity_;

  /// a default production cost
  double default_cost_;

 private:
  /// a collection of commodities and their production capacities
  std::map<Commodity, CommodInfo, CommodityCompare> produced_commodities_;

  // #include "commodity_producer_tests.h"
  // friend class CommodityProducerTests;
  // @MJGFlag - removed for the same reason as above
};

} // namespace toolkit
} // namespace cyclus
#endif  // CYCLUS_SRC_COMMODITY_PRODUCER_H_
