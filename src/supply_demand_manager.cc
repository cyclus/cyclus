#include "supply_demand_manager.h"

#include "error.h"

namespace cyclus {
namespace supply_demand {

// -------------------------------------------------------------------
SupplyDemandManager::SupplyDemandManager() {}

// -------------------------------------------------------------------
SupplyDemandManager::~SupplyDemandManager() {}

// -------------------------------------------------------------------
void SupplyDemandManager::RegisterCommodity(Commodity& commodity,
                                            SymFunction::Ptr fp) {
  demand_functions_.insert(std::make_pair(commodity, fp));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SupplyDemandManager::ManagesCommodity(Commodity& commodity) {
  return (demand_functions_.find(commodity) !=
          demand_functions_.end());
}

// -------------------------------------------------------------------
void SupplyDemandManager::RegisterProducerManager(
  supply_demand::CommodityProducerManager* cpm) {
  managers_.insert(cpm);
}

// -------------------------------------------------------------------
void SupplyDemandManager::UnRegisterProducerManager(
  supply_demand::CommodityProducerManager* cpm) {
  managers_.erase(cpm);
}

// -------------------------------------------------------------------
double SupplyDemandManager::Supply(Commodity& commodity) {
  ThrowErrorIfCommodityNotManaged(commodity);
  double value = 0.0;
  std::set<CommodityProducerManager*>::iterator it;
  for (it = managers_.begin(); it != managers_.end(); it++) {
    value += (*it)->TotalProductionCapacity(commodity);
  }
  return value;
}

// -------------------------------------------------------------------
double SupplyDemandManager::Demand(Commodity& commodity, int time) {
  ThrowErrorIfCommodityNotManaged(commodity);
  return demand_functions_[commodity]->value(time);
}

// -------------------------------------------------------------------
SymFunction::Ptr SupplyDemandManager::DemandFunction(Commodity& commodity) {
  ThrowErrorIfCommodityNotManaged(commodity);
  return demand_functions_[commodity];
}

// -------------------------------------------------------------------
void SupplyDemandManager::ThrowErrorIfCommodityNotManaged(
  Commodity& commodity) {
  if (!ManagesCommodity(commodity)) {
    throw KeyError("SDManager does not manage demand for "
                   + commodity.name());
  }
}
} // namespace supply_demand
} // namespace cyclus
