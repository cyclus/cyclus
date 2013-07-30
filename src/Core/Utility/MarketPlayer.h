// MarketPlayer.h
#ifndef MARKETPLAYER_H
#define MARKETPLAYER_H

#include <utility>
#include <map>
#include <list>
#include <vector>
#include <sstream>

#include "SupplyDemand.h"

namespace cyclus {
class MarketPlayer;
} // namespace cyclus

#include "MarketPlayerManager.h"

namespace cyclus {

/// an iterator for the manager map
typedef std::map<Commodity,
  std::vector<MarketPlayerManager*> >::iterator ManagerIterator;
/// an iterator for the production map
typedef std::map<Commodity,double>::iterator ProductionIterator;

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
  void registerCommodity(Commodity& commod);

  /**
     set the production capacity for a commodity
     @param value the value of the production capacity
     @param commod the produced commodity
   */
  void setProductionCapacity(double value,Commodity& commod);

  /**
     return the production capacity for a commodity
     @param commod the produced commodity
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
     @param commod the commodity of the market to enter
   */
  void enterMarket(Commodity& commod);

  /**
     inform the manager that you are leaving the market for a 
     commodity
     @param commod the commodity of the market to leave
   */
  void leaveMarket(Commodity& commod);

 private:
  /// the production capacity of each commodity
  std::map<Commodity,double,CommodityCompare> production_capacity_;

  /// the managers of each commodity
  std::map<Commodity,std::vector<MarketPlayerManager*>,
    CommodityCompare> managers_;
  
  /**
     throws an error saying the commodity is not registered
     @param commod the commodity that is ot registered
   */
  void throwRegistrationException(Commodity& commod);

  /**
     Verifies that commod is in production_capacity_. If not, 
     throwRegistrationException() is called.
     @param commod the commodity to verify
     @return an iterator to the value/key pair in production_capacity_
   */
  ProductionIterator checkCommodityProduction(Commodity& commod);

  /**
     Verifies that commod is in managers_. If not, 
     throwRegistrationException() is called.
     @param commod the commodity to verify
     @return an iterator to the value/key pair in managers_
   */
  ManagerIterator checkCommodityManagement(Commodity& commod);
};
} // namespace cyclus

#include "CycException.h"

namespace cyclus {
/**
   an exception class for mismatched commodities and managers
 */
class CycCommodityMismatchError: public CycException {
  public: CycCommodityMismatchError(std::string msg) : 
  CycException(msg) {};
};
} // namespace cyclus

#endif
