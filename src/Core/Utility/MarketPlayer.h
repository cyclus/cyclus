// MarketPlayer.h
#ifndef MARKETPLAYER_H
#define MARKETPLAYER_H

#include "SupplyDemand.h"

#include <map>

/**
   A MarketPlayer is an agent mixin that allows for querying
   capability regarding its market-based interactions. For each
   commodity which the player produces, this class provides an 
   interface for its current production capacity.

   Additionally, an interface is provided to inform a 
   MarketPlayerManager as to when the Player enters and leaves the
   market.
 */
class MarketPlayer {
 public:
  /**
     constructor
   */
  MarketPlayer();

  /**
     register a commodity that is produced
     @param the produced commodity
   */
  void registerCommodity(Commodity& commod);

  /**
     set the production capacity for a commodity
     @param the produced commodity
   */
  void setProductionCapacity(Commodity& commod);

  /**
     return the production capacity for a commodity
     @param the produced commodity
   */
  double productionCapacity(Commodity& commod);

  /**
     registers a manager for a commodity
     @param m the manager to register
     @param commod the commodity this manager is managing
   */
  void registerManager(MarketPlayerManager* m, Commodity& commod);

  /**
     inform the manager that you are entering the market for a 
     commodity
     @param the commodity of the market to enter
   */
  void enterMarket(Commodity& commod);

  /**
     inform the manager that you are leaving the market for a 
     commodity
     @param the commodity of the market to leave
   */
  void leaveMarket(Commodity& commod);

 private:
  std::map<Commodity, std::vector<MarketPlayerManager*> > managers_;
  std::map<Commodity, double> production_capacity_;
};

#endif
