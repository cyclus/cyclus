#include "MarketPlayer.h"

#include <sstream>
#include "error.h"

namespace cyclus {

// -------------------------------------------------------------------
MarketPlayer::MarketPlayer() {
  managers_ = std::map<Commodity, std::vector<MarketPlayerManager*>,
                  CommodityCompare> ();
  production_capacity_ = std::map<Commodity,double,CommodityCompare>();
}

// -------------------------------------------------------------------
void MarketPlayer::RegisterCommodity(Commodity& commod) {
  std::vector<MarketPlayerManager*> v;
  managers_.insert(std::make_pair(commod,v));  
  production_capacity_.insert(std::make_pair(commod,0.0));
}

// -------------------------------------------------------------------
void MarketPlayer::SetProductionCapacity(double value,
                                         Commodity& commod) {
  ProductionIterator pi = CheckCommodityProduction(commod);
  pi->second = value;
}

// -------------------------------------------------------------------
double MarketPlayer::ProductionCapacity(Commodity& commod) {
  ProductionIterator pi = CheckCommodityProduction(commod);
  return pi->second;
}

// -------------------------------------------------------------------
void MarketPlayer::RegisterManager(MarketPlayerManager* m, 
                                   Commodity& commod) {
  ManagerIterator mi = CheckCommodityManagement(commod);
  if (m->commodity() != commod) {
    std::stringstream ss("");
    ss << "Cannot register a manager of " << m->commodity().name() 
       << " with the commodity: " << commod.name();
    throw ValueError(ss.str());
  }
  mi->second.push_back(m);
}

// -------------------------------------------------------------------
void MarketPlayer::EnterMarket(Commodity& commod) {
  ManagerIterator mi = CheckCommodityManagement(commod);
  std::vector<MarketPlayerManager*> v = mi->second;
  for (int i = 0; i < v.size(); i++) {
    v.at(i)->PlayerEnteringMarket(this);
  }
}

// -------------------------------------------------------------------
void MarketPlayer::LeaveMarket(Commodity& commod) {
  ManagerIterator mi = CheckCommodityManagement(commod);
  std::vector<MarketPlayerManager*> v = mi->second;
  for (int i = 0; i < v.size(); i++) {
    v.at(i)->PlayerLeavingMarket(this);
  }
}

// -------------------------------------------------------------------
ProductionIterator 
MarketPlayer::CheckCommodityProduction(Commodity& commod) {
  ProductionIterator pi = production_capacity_.find(commod);
  if (pi == production_capacity_.end()) {
    ThrowRegistrationException(commod);
  }
  return pi;
}

// -------------------------------------------------------------------
ManagerIterator 
MarketPlayer::CheckCommodityManagement(Commodity& commod) {
  ManagerIterator mi = managers_.find(commod);
  if (mi == managers_.end()) {
    ThrowRegistrationException(commod);
  }
  return mi;
}

// -------------------------------------------------------------------
void MarketPlayer::ThrowRegistrationException(Commodity& commod) {
  std::stringstream ss ("");
  ss << "Commodity " << commod.name() << " is not registered with "
     << " this MarketPlayer.";
  throw KeyError(ss.str());
}
} // namespace cyclus
