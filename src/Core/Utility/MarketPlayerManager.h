// MarketPlayerManager.h
#ifndef MARKETPLAYERMANAGER_H
#define MARKETPLAYERMANAGER_H

#include "SupplyDemand.h"

#include <set>

class MarketPlayerManager;
#include "MarketPlayer.h"

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
  void setCommodity(Commodity& commod);

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
  double playerProductionCapacity();
  
 private:
  /// the commodity associated with this manager
  Commodity* commod_;

  /// the players associated with this manager
  std::set<MarketPlayer*> players_;

  /**
     add a player from the list of players
     @param m the player to add
   */
  void playerEnteringMarket(MarketPlayer* m);

  /**
     remove a player from the list of players
     @param m the player to remove
   */
  void playerLeavingMarket(MarketPlayer* m);

  /// MarketPlayer is a friend class for access to enter/leaving
  friend class MarketPlayer;
};

#endif
