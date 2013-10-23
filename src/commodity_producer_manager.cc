#include "commodity_producer_manager.h"

#include "error.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CommodityProducerManager::CommodityProducerManager() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CommodityProducerManager::~CommodityProducerManager() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double CommodityProducerManager::TotalProductionCapacity(Commodity& commodity) {
  double total = 0.0;
  std::set<CommodityProducer*>::iterator it;
  for (it = producers_.begin(); it != producers_.end(); it++) {
    if ((*it)->ProducesCommodity(commodity)) {
      total += (*it)->ProductionCapacity(commodity);
    }
  }
  return total;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityProducerManager::RegisterProducer(
  CommodityProducer* producer) {
  if (producers_.find(producer) != producers_.end()) {
    throw KeyError("A manager is trying to register a producer twice.");
  } else {
    producers_.insert(producer);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommodityProducerManager::UnRegisterProducer(
  CommodityProducer* producer) {
  if (producers_.find(producer) == producers_.end()) {
    throw KeyError("A manager is trying to unregister a producer not originally registered with it.");
  } else {
    producers_.erase(producer);
  }
}
 
} // namespace cyclus

