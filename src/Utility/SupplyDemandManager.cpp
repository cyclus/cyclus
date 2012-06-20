#include "SupplyDemandManager.h"

using namespace std;

// --------------------------------------------------------------------------------------
SupplyDemandManager::SupplyDemandManager() {
  commodities_ = map<Commodity,CommodityInformation,CommodityCompare>();
}

// --------------------------------------------------------------------------------------
void SupplyDemandManager::registerCommodity(const Commodity& commodity, const FunctionPtr fp, 
                                          const std::vector<Producer>& producers) {
  commodities_.insert(pair<Commodity,CommodityInformation>
                   (commodity,CommodityInformation(commodity,fp,producers)));  
}

// --------------------------------------------------------------------------------------
void SupplyDemandManager::registerProducer(const Commodity& commodity, 
                                           const Producer& producer) {
  commodities_.find(commodity)->second.registerProducer(producer);
}

// --------------------------------------------------------------------------------------
double SupplyDemandManager::demand(const Commodity& commodity, int time) {
  return commodities_.find(commodity)->second.demand(time);
}

// --------------------------------------------------------------------------------------
double SupplyDemandManager::supply(const Commodity& commodity) {
  return commodities_.find(commodity)->second.supply();
}

// --------------------------------------------------------------------------------------
void SupplyDemandManager::increaseSupply(const Commodity& commodity, double amt) { 
  commodities_.find(commodity)->second.increaseSupply(amt); 
}

// --------------------------------------------------------------------------------------
int SupplyDemandManager::nProducers(const Commodity& commodity) {
  return commodities_.find(commodity)->second.nProducers(); 
}

// --------------------------------------------------------------------------------------
Producer* SupplyDemandManager::producer(const Commodity& commodity, int index) {
  return commodities_.find(commodity)->second.producer(index); 
}
