// market_player.h
#ifndef MARKETPLAYER_H
#define MARKETPLAYER_H

#include <utility>
#include <map>
#include <list>
#include <vector>
#include <sstream>

#include "supply_demand.h"

namespace cyclus {
class MarketPlayer;
} // namespace cyclus

#include "market_player_manager.h"

namespace cyclus {

/// an iterator for the manager map
typedef std::map < Commodity,
        std::vector<MarketPlayerManager*> >::iterator ManagerIterator;

/// an iterator for the production map
typedef std::map<Commodity, double>::iterator ProductionIterator;

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
     destructor
   */
  virtual ~MarketPlayer() {};

  /**
     register a commodity that is produced
     @param commod the produced commodity
   */
  void RegisterCommodity(Commodity& commod);

  /**
     set the production capacity for a commodity
     @param value the value of the production capacity
     @param commod the produced commodity
   */
  void SetProductionCapacity(double value, Commodity& commod);

  /**
     return the production capacity for a commodity
     @param commod the produced commodity
   */
  double ProductionCapacity(Commodity& commod);

  /**
     registers a manager for a commodity
     @param m the manager to register
     @param commod the commodity this manager is managing
   */
  void RegisterManager(MarketPlayerManager* m, Commodity& commod);

  /**
     inform the manager that you are entering the market for a
     commodity
     @param commod the commodity of the market to enter
   */
  void EnterMarket(Commodity& commod);

  /**
     inform the manager that you are leaving the market for a
     commodity
     @param commod the commodity of the market to leave
   */
  void LeaveMarket(Commodity& commod);

 private:
  /// the production capacity of each commodity
  std::map<Commodity, double, CommodityCompare> production_capacity_;

  /// the managers of each commodity
  std::map < Commodity, std::vector<MarketPlayerManager*>,
      CommodityCompare > managers_;

  /**
     throws an error saying the commodity is not registered
     @param commod the commodity that is ot registered
   */
  void ThrowRegistrationException(Commodity& commod);

  /**
     Verifies that commod is in production_capacity_. If not,
     ThrowRegistrationException() is called.
     @param commod the commodity to verify
     @return an iterator to the value/key pair in production_capacity_
   */
  ProductionIterator CheckCommodityProduction(Commodity& commod);

  /**
     Verifies that commod is in managers_. If not,
     ThrowRegistrationException() is called.
     @param commod the commodity to verify
     @return an iterator to the value/key pair in managers_
   */
  ManagerIterator CheckCommodityManagement(Commodity& commod);
};

} // namespace cyclus
#endif
