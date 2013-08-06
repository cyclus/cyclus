// market_player_manager.h
#ifndef MARKETPLAYERMANAGER_H
#define MARKETPLAYERMANAGER_H

#include <set>

#include "supply_demand.h"

namespace cyclus {
class MarketPlayerManager;
} // namespace cyclus

#include "market_player.h"


namespace cyclus {

class MarketPlayerManager {
 public:
  /**
     constructor
   */
  MarketPlayerManager();

  /**
     destructor
   */
  virtual ~MarketPlayerManager() {};
  
  /**
     set's the commodity associated with this manager
   */
  void SetCommodity(Commodity& commod);

  /**
     return the commodity associated with this manager
     @return the commodity associated with this manager
   */
  Commodity commodity();

  /**
     returns the total production capacity of all players in the
     market
     @return the total production capacity of the market
   */
  double PlayerProductionCapacity();
  
 private:
  /// the commodity associated with this manager
  Commodity* commod_;

  /// the players associated with this manager
  std::set<MarketPlayer*> players_;

  /**
     add a player from the list of players
     @param m the player to add
   */
  void PlayerEnteringMarket(MarketPlayer* m);

  /**
     remove a player from the list of players
     @param m the player to remove
   */
  void PlayerLeavingMarket(MarketPlayer* m);

  /// MarketPlayer is a friend class for access to enter/leaving
  friend class MarketPlayer;
};
} // namespace cyclus
#endif
