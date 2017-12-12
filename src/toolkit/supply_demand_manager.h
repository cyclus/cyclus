#ifndef CYCLUS_SRC_TOOLKIT_SUPPLY_DEMAND_MANAGER_H_
#define CYCLUS_SRC_TOOLKIT_SUPPLY_DEMAND_MANAGER_H_

#include "agent_managed.h"
#include "commodity.h"
#include "commodity_producer_manager.h"
#include "symbolic_functions.h"

#include <map>
#include <set>

namespace cyclus {
namespace toolkit {

/// This is a manager class that manages a set of commodities. Those
/// commodities have a certain demand function associated with them
/// and a list of producers who can produce the commodities.
///
/// The SupplyDemandManager simply keeps track of this information and
/// provides the demand and supply of a commodity at a given time.
/// What to do with this information is left to the user of the
/// SupplyDemandManager.
class SupplyDemandManager : public AgentManaged {
 public:
  SupplyDemandManager(Agent* agent = NULL) : AgentManaged(agent) {}
  virtual ~SupplyDemandManager() {}

  /// Register a new commodity with the manager, along with all the
  /// necessary information.
  /// @param commodity the commodity
  /// @param demand a smart pointer to the demand function
  inline void RegisterCommodity(Commodity& commodity,
                                SymFunction::Ptr demand) {
    demand_functions_.insert(std::make_pair(commodity, demand));
  }

  /// @return true if the demand for a commodity is managed by this entity
  /// @param commodity the commodity in question
  inline bool ManagesCommodity(Commodity& commodity) {
    return demand_functions_.find(commodity) != demand_functions_.end();
  }

  /// Adds a commodity producer manager to the set of producer managers
  inline void RegisterProducerManager(CommodityProducerManager* cpm) {
    managers_.insert(cpm);
  }

  /// Removes a commodity producer manager from the set of producer
  /// managers
  inline void UnregisterProducerManager(CommodityProducerManager* cpm) {
    managers_.erase(cpm);
  }

  /// The demand for a commodity at a given time
  /// @param commodity the commodity
  /// @param time the time
  inline double Demand(Commodity& commodity, int time) {
    return demand_functions_[commodity]->value(time);
  }

  /// Returns the demand function for a commodity
  /// @param commodity the commodity being queried
  inline SymFunction::Ptr DemandFunction(Commodity& commodity) {
    return demand_functions_[commodity];
  }

  /// Returns the current supply of a commodity
  /// @param commodity the commodity
  /// @return the current supply of the commodity
  double Supply(Commodity& commodity);

 private:
  /// A container of all demand functions known to the manager
  std::map<Commodity, SymFunction::Ptr, CommodityCompare> demand_functions_;

  /// A container of all production managers known to the manager
  std::set<CommodityProducerManager*> managers_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_SUPPLY_DEMAND_MANAGER_H_
