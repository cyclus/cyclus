#include "SupplyDemandManager.h"

#include <sstream>

using namespace std;

// -------------------------------------------------------------------
SupplyDemandManager::SupplyDemandManager() {
  commodities_ = map<Commodity,CommodityInformation,CommodityCompare>();
}

// -------------------------------------------------------------------
void 
SupplyDemandManager::registerCommodity(const Commodity& commodity, 
                                       const FunctionPtr fp, 
                                       const std::vector<Producer>& producers) {
  commodities_.insert(pair<Commodity,CommodityInformation>
                   (commodity,CommodityInformation(commodity,fp,producers)));  
}

// -------------------------------------------------------------------
void SupplyDemandManager::registerProducer(const Commodity& commodity,
                                           const Producer& producer) {
  commodities_.find(commodity)->second.registerProducer(producer);
}

// -------------------------------------------------------------------
double SupplyDemandManager::demand(const Commodity& commodity, 
                                   int time) {
  return commodities_.find(commodity)->second.demand(time);
}

// -------------------------------------------------------------------
FunctionPtr 
SupplyDemandManager::demandFunction(const Commodity& commodity) {
  return commodities_.find(commodity)->second.demandFunction();
}

// -------------------------------------------------------------------
double SupplyDemandManager::supply(const Commodity& commodity) {
  return commodities_.find(commodity)->second.supply();
}

// -------------------------------------------------------------------
void SupplyDemandManager::increaseSupply(const Commodity& commodity, 
                                         double amt) { 
  commodities_.find(commodity)->second.increaseSupply(amt); 
}

// -------------------------------------------------------------------
void SupplyDemandManager::decreaseSupply(const Commodity& commodity, 
                                         double amt) { 
  if (supply(commodity) - amt < 0) {
    stringstream ss("");
    ss << "Cannot decrease supply of " << commodity.name()
       << " from " << supply(commodity) << " by " << amt
       << " because a negative value would result.";
    throw CycNegativeValueException(ss.str());
  }
  else {
    commodities_.find(commodity)->second.decreaseSupply(amt); 
  }
}

// -------------------------------------------------------------------
int SupplyDemandManager::nProducers(const Commodity& commodity) {
  return commodities_.find(commodity)->second.nProducers(); 
}

// -------------------------------------------------------------------
Producer* SupplyDemandManager::producer(const Commodity& commodity, 
                                        int index) {
  return commodities_.find(commodity)->second.producer(index); 
}
