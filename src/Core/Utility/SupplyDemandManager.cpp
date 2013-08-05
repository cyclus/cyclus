#include "SupplyDemandManager.h"

#include "error.h"

namespace cyclus {
namespace supply_demand {

// -------------------------------------------------------------------
SupplyDemandManager::SupplyDemandManager() {}

// -------------------------------------------------------------------
SupplyDemandManager::~SupplyDemandManager() {}

// -------------------------------------------------------------------
void SupplyDemandManager::registerCommodity(Commodity& commodity, 
                                            FunctionPtr fp) 
{
  demand_functions_.insert(std::make_pair(commodity,fp));  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool SupplyDemandManager::managesCommodity(Commodity& commodity)
{
  return (demand_functions_.find(commodity) != 
          demand_functions_.end());
}

// -------------------------------------------------------------------
void SupplyDemandManager::registerProducerManager(supply_demand::CommodityProducerManager* cpm) 
{
  managers_.insert(cpm);
}

// -------------------------------------------------------------------
void SupplyDemandManager::unRegisterProducerManager(supply_demand::CommodityProducerManager* cpm) 
{
  managers_.erase(cpm);
}

// -------------------------------------------------------------------
double SupplyDemandManager::supply(Commodity& commodity) 
{
  throwErrorIfCommodityNotManaged(commodity);
  double value = 0.0;
  std::set<CommodityProducerManager*>::iterator it;
  for (it = managers_.begin(); it != managers_.end(); it++) 
    {
      value += (*it)->totalProductionCapacity(commodity);
    }
  return value;
}

// -------------------------------------------------------------------
double SupplyDemandManager::demand(Commodity& commodity,int time) 
{
  throwErrorIfCommodityNotManaged(commodity);
  return demand_functions_[commodity]->value(time);
}

// -------------------------------------------------------------------
FunctionPtr SupplyDemandManager::demandFunction(Commodity& commodity) 
{
  throwErrorIfCommodityNotManaged(commodity);
  return demand_functions_[commodity];
}

// -------------------------------------------------------------------
void SupplyDemandManager::throwErrorIfCommodityNotManaged(Commodity& commodity)
{
  if(!managesCommodity(commodity))
    {
      throw KeyError("SDManager does not manage demand for " 
                                      + commodity.name());
    }
}
} // namespace supply_demand
} // namespace cyclus
