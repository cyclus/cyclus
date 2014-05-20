#ifndef CYCLUS_SRC_COMMODITY_PRODUCER_H_
#define CYCLUS_SRC_COMMODITY_PRODUCER_H_

#include <map>
#include <set>

#include "cyc_limits.h"
#include "commodity.h"

namespace cyclus {
namespace toolkit {

/// a container to hold information about a commodity
struct CommodInfo {
  CommodInfo(double default_capacity = 0,
             double default_cost = kModifierLimit);
  double capacity;
  double cost;
};

/// a mixin to provide information about produced commodities
class CommodityProducer {
 public:
  CommodityProducer(double default_capacity = 0,
                    double default_cost = kModifierLimit);
  virtual ~CommodityProducer();

  /// @param commodity the commodity in question
  /// @return true if the commodity is produced by this entity
  inline bool Produces(const Commodity& commodity) const {
    return commodities_.find(commodity) != commodities_.end();
  }

  /// @param commodity the commodity in question
  /// @return the production capacity for a commodity
  inline double Capacity(const Commodity& commodity) {
    return commodities_[commodity].capacity; 
  }

  /// @return the cost to produce a commodity at a given capacity
  /// @param commodity the commodity in question
  inline double Cost(const Commodity& commodity) {
    return commodities_[commodity].cost;
  }

  /// set the production capacity for a given commodity
  /// @param commodity the commodity being produced
  /// @param capacity the production capacity
  inline void SetCapacity(const Commodity& commodity, double capacity) {
    commodities_[commodity].capacity = capacity;
  }

  /// set the production cost for a given commodity
  /// @param commodity the commodity being produced
  /// @param cost the production cost
  inline void SetCost(const Commodity& commodity, double cost) {
    commodities_[commodity].cost = cost;
  }

  /// register a commodity as being produced by this object
  /// @param commodity the commodity being produced
  inline void Add(const Commodity& commodity) {
    Add(commodity, CommodInfo(default_capacity_, default_cost_));
  }

  /// register a commodity as being produced by this object and set
  /// its relevant info
  /// @param commodity the commodity being produced
  /// @param info the information describing the commodity
  inline void Add(const Commodity& commodity, const CommodInfo& info) {
    commodities_.insert(std::make_pair(commodity, info));
  }

  /// unregister a commodity as being produced by this object
  /// @param commodity the commodity being produced
  inline void Rm(const Commodity& commodity) {
    commodities_.erase(commodity);
  }

  /// @return the set of commodities produced by this producers
  std::set<Commodity, CommodityCompare> ProducedCommodities();

  /// add all commodities produced by a source
  /// @param source the original commodity producer
  void Copy(CommodityProducer* source);

 private:
  /// a collection of commodities and their production capacities
  std::map<Commodity, CommodInfo, CommodityCompare> commodities_;

  /// a default production capacity
  double default_capacity_;

  /// a default production cost
  double default_cost_;
};

} // namespace toolkit
} // namespace cyclus
#endif  // CYCLUS_SRC_COMMODITY_PRODUCER_H_
