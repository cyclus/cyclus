#include "MarketPlayerManager.h"

namespace cyclus {

// -------------------------------------------------------------------
MarketPlayerManager::MarketPlayerManager() {
  players_ = std::set<MarketPlayer*>();
}

// -------------------------------------------------------------------
void MarketPlayerManager::setCommodity(Commodity& commod) {
  commod_ = &commod;
}

// -------------------------------------------------------------------
Commodity MarketPlayerManager::commodity() {
  return *commod_;
}

// -------------------------------------------------------------------
void MarketPlayerManager::playerEnteringMarket(MarketPlayer* m) {
  players_.insert(m);
}

// -------------------------------------------------------------------
void MarketPlayerManager::playerLeavingMarket(MarketPlayer* m) {
    players_.erase(m);
}

// -------------------------------------------------------------------
double MarketPlayerManager::playerProductionCapacity() {
  double value = 0;
  std::set<MarketPlayer*>::iterator it;
  for (it = players_.begin(); it != players_.end(); it++) {
    value += (*it)->productionCapacity(*commod_);
  }
  return value;
}
} // namespace cyclus
