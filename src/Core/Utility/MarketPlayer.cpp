#include "MarketPlayer.h"

namespace cyclus {

// -------------------------------------------------------------------
MarketPlayer::MarketPlayer() {
  managers_ = std::map<Commodity, std::vector<MarketPlayerManager*>,
                  CommodityCompare> ();
  production_capacity_ = std::map<Commodity,double,CommodityCompare>();
}

// -------------------------------------------------------------------
void MarketPlayer::registerCommodity(Commodity& commod) {
  std::vector<MarketPlayerManager*> v;
  managers_.insert(std::make_pair(commod,v));  
  production_capacity_.insert(std::make_pair(commod,0.0));
}

// -------------------------------------------------------------------
void MarketPlayer::setProductionCapacity(double value,
                                         Commodity& commod) {
  ProductionIterator pi = checkCommodityProduction(commod);
  pi->second = value;
}

// -------------------------------------------------------------------
double MarketPlayer::productionCapacity(Commodity& commod) {
  ProductionIterator pi = checkCommodityProduction(commod);
  return pi->second;
}

// -------------------------------------------------------------------
void MarketPlayer::registerManager(MarketPlayerManager* m, 
                                   Commodity& commod) {
  ManagerIterator mi = checkCommodityManagement(commod);
  if (m->commodity() != commod) {
    stringstream ss("");
    ss << "Cannot register a manager of " << m->commodity().name() 
       << " with the commodity: " << commod.name();
    throw CycCommodityMismatchError(ss.str());
  }
  mi->second.push_back(m);
}

// -------------------------------------------------------------------
void MarketPlayer::enterMarket(Commodity& commod) {
  ManagerIterator mi = checkCommodityManagement(commod);
  vector<MarketPlayerManager*> v = mi->second;
  for (int i = 0; i < v.size(); i++) {
    v.at(i)->playerEnteringMarket(this);
  }
}

// -------------------------------------------------------------------
void MarketPlayer::leaveMarket(Commodity& commod) {
  ManagerIterator mi = checkCommodityManagement(commod);
  std::vector<MarketPlayerManager*> v = mi->second;
  for (int i = 0; i < v.size(); i++) {
    v.at(i)->playerLeavingMarket(this);
  }
}

// -------------------------------------------------------------------
ProductionIterator 
MarketPlayer::checkCommodityProduction(Commodity& commod) {
  ProductionIterator pi = production_capacity_.find(commod);
  if (pi == production_capacity_.end()) {
    throwRegistrationException(commod);
  }
  return pi;
}

// -------------------------------------------------------------------
ManagerIterator 
MarketPlayer::checkCommodityManagement(Commodity& commod) {
  ManagerIterator mi = managers_.find(commod);
  if (mi == managers_.end()) {
    throwRegistrationException(commod);
  }
  return mi;
}

// -------------------------------------------------------------------
void MarketPlayer::throwRegistrationException(Commodity& commod) {
  std::stringstream ss("");
  ss << "Commodity " << commod.name() << " is not registered with "
     << " this MarketPlayer.";
  throw CycKeyException(ss.str());
}
} // namespace cyclus
