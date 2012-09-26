#include "CommodityProducerManager.h"

#include "CycException.h"

using namespace std;
using namespace SupplyDemand;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityProducerManager::CommodityProducerManager() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityProducerManager::~CommodityProducerManager() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducerManager::totalProductionCapacity(Commodity& commodity)
{
  double total = 0.0;
  set<CommodityProducer*>::iterator it;
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
void CommodityProducerManager::registerProducer(SupplyDemand::CommodityProducer* producer) {
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
void CommodityProducerManager::unRegisterProducer(SupplyDemand::CommodityProducer* producer) {
  if (producers_.find(producer) == producers_.end())
    {
      throw CycNotRegisteredException("A manager is trying to unregister a producer not originally registered with it.");
    }
  else
    {
      producers_.erase(producer);
    }
}
