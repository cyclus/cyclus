#include "CommodityProducerManager.h"

#include "CycException.h"

//using namespace supply_demand;

namespace cyclus {
namespace supply_demand {
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityProducerManager::CommodityProducerManager() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityProducerManager::~CommodityProducerManager() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducerManager::totalProductionCapacity(Commodity& commodity)
{
  double total = 0.0;
  std::set<CommodityProducer*>::iterator it;
  for (it = producers_.begin(); it != producers_.end(); it++)
    {
      if ( (*it)->producesCommodity(commodity) )
        {
          total += (*it)->productionCapacity(commodity);
        }
    }
  return total;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManager::registerProducer(supply_demand::CommodityProducer* producer) {
  if (producers_.find(producer) != producers_.end())
    {
      throw CycDoubleRegistrationException("A manager is trying to register a producer twice.");
    }
  else
    {
      producers_.insert(producer);
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducerManager::unRegisterProducer(supply_demand::CommodityProducer* producer) {
  if (producers_.find(producer) == producers_.end())
    {
      throw CycNotRegisteredException("A manager is trying to unregister a producer not originally registered with it.");
    }
  else
    {
      producers_.erase(producer);
    }
}
} // namespace supply_demand
} // namespace cyclus

