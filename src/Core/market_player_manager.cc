#include "market_player_manager.h"

namespace cyclus {

// -------------------------------------------------------------------
MarketPlayerManager::MarketPlayerManager() {
  players_ = std::set<MarketPlayer*>();
}

// -------------------------------------------------------------------
void MarketPlayerManager::SetCommodity(Commodity& commod) {
  commod_ = &commod;
}

// -------------------------------------------------------------------
Commodity MarketPlayerManager::commodity() {
  return *commod_;
}

// -------------------------------------------------------------------
void MarketPlayerManager::PlayerEnteringMarket(MarketPlayer* m) {
  players_.insert(m);
}

// -------------------------------------------------------------------
void MarketPlayerManager::PlayerLeavingMarket(MarketPlayer* m) {
  players_.erase(m);
}

// -------------------------------------------------------------------
double MarketPlayerManager::PlayerProductionCapacity() {
  double value = 0;
  std::set<MarketPlayer*>::iterator it;
  for (it = players_.begin(); it != players_.end(); it++) {
    value += (*it)->ProductionCapacity(*commod_);
  }
  return value;
}
} // namespace cyclus
