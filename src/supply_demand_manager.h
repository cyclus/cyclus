#ifndef SUPPLYDEMANDMANAGER_H
#define SUPPLYDEMANDMANAGER_H

#include "commodity.h"
#include "commodity_producer_manager.h"
#include "symbolic_functions.h"

#include <map>
#include <set>

namespace cyclus {
namespace supply_demand {
/**
   This is a manager class that manages a set of commodities. Those
   commodities have a certain demand function associated with them
   and a list of producers who can produce the commodities.

   The SupplyDemandManager simply keeps track of this information and
   provides the demand and supply of a commodity at a given time.
   What to do with this information is left to the user of the
   SupplyDemandManager.
 */
class SupplyDemandManager {
 public:
  /**
     constructor
   */
  SupplyDemandManager();

  /**
     virtual destructor
   */
  virtual ~SupplyDemandManager();

  /**
     register a new commodity with the manager, along with all the
     necessary information
     @param commodity the commodity
     @param demand a smart pointer to the demand function
   */
  void RegisterCommodity(Commodity& commodity, SymFunction::Ptr demand);

  /**
     @return true if the demand for a commodity is managed by this entity
     @param commodity the commodity in question
  */
  bool ManagesCommodity(Commodity& commodity);

  /**
     adds a commodity producer manager to the set of producer managers
   */
  void RegisterProducerManager(supply_demand::CommodityProducerManager* cpm);

  /**
     removes a commodity producer manager from the set of producer
     managers
   */
  void UnRegisterProducerManager(supply_demand::CommodityProducerManager* cpm);

  /**
     returns the current supply of a commodity
     @param commodity the commodity
     @return the current supply of the commodity
   */
  double Supply(Commodity& commodity);

  /**
     the demand for a commodity at a given time
     @param commodity the commodity
     @param time the time
   */
  double Demand(Commodity& commodity, int time);

  /**
     returns the demand function for a commodity
     @param commodity the commodity being queried
   */
  SymFunction::Ptr DemandFunction(Commodity& commodity);

  // protected: @MJGFlag - should be protected. revise when tests can
  // be found by classes in the Utility folder
  /**
     checks if ManagesCommodity() is true. if it is false, an
     error is thrown.
     @param commodity the commodity in question
  */
  void ThrowErrorIfCommodityNotManaged(Commodity& commodity);

 private:
  /// a container of all demand functions known to the manager
  std::map<Commodity, SymFunction::Ptr, CommodityCompare> demand_functions_;

  /// a container of all production managers known to the manager
  std::set<supply_demand::CommodityProducerManager*> managers_;
};
} // namespace supply_demand
} // namespace cyclus
#endif
