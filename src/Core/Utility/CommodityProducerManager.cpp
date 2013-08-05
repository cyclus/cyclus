#include "CommodityProducerManager.h"

#include "error.h"

//using namespace supply_demand;

namespace cyclus {
namespace supply_demand {
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityProducerManager::CommodityProducerManager() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityProducerManager::~CommodityProducerManager() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducerManager::TotalProductionCapacity(Commodity& commodity)
{
  double total = 0.0;
  std::set<CommodityProducer*>::iterator it;
  for (it = producers_.begin(); it != producers_.end(); it++)
    {
      if ( (*it)->ProducesCommodity(commodity) )
        {
          total += (*it)->ProductionCapacity(commodity);
        }
    }
  return total;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManager::RegisterProducer(supply_demand::CommodityProducer* producer) {
  if (producers_.find(producer) != producers_.end())
    {
      throw KeyError("A manager is trying to register a producer twice.");
    }
  else
    {
      producers_.insert(producer);
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManager::UnRegisterProducer(supply_demand::CommodityProducer* producer) {
  if (producers_.find(producer) == producers_.end())
    {
      throw KeyError("A manager is trying to unregister a producer not originally registered with it.");
    }
  else
    {
      producers_.erase(producer);
    }
}
} // namespace supply_demand
} // namespace cyclus

