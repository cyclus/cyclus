#include "SupplyDemandManager.h"

#include <utility>
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
void 
SupplyDemandManager::
registerPlayerManager(const Commodity& commodity, 
                      MarketPlayerManager* m) {
  map<Commodity,set<MarketPlayerManager*>,CommodityCompare>::iterator 
    it = player_managers_.find(commodity);
  if (it == player_managers_.end()) {
    set<MarketPlayerManager*> s;
    player_managers_.insert(make_pair(commodity,s));
  }
  player_managers_[commodity].insert(m);
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
  double value = 0;
  set<MarketPlayerManager*>::iterator it;
  set<MarketPlayerManager*> managers = player_managers_[commodity];
  for (it = managers.begin(); it != managers.end(); it++) {
    value += (*it)->playerProductionCapacity();
  }
  return value;
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
