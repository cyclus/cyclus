#ifndef SUPPLYDEMANDMANAGER_H
#define SUPPLYDEMANDMANAGER_H

#include "SupplyDemand.h"
#include "SymbolicFunctions.h"
#include "MarketPlayerManager.h"

#include <vector>
#include <map>
#include <set>

/**
   This is a manager class that manages a set of commodities. Those 
   commodities have a certain demand function associated with them 
   and a list of producers who can produce the commodities.

   The SupplyDemandManager simply keeps track of this information and 
   provides the demand and supply of a commodity at a given time. 
   What to do with this information is left to the user of the 
   SupplyDemandManager.
 */
class SupplyDemandManager {
 public:
  /**
     constructor
   */
  SupplyDemandManager();

  /**
     register a new commodity with the manager, along with all the 
     necessary information
     @param commodity the commodity
     @param fp a smart pointer to the demand function
     @param producers the list of producers of commodity
   */
  void registerCommodity(const Commodity& commodity, 
                         const FunctionPtr fp, 
                         const std::vector<Producer>& producers);

  /**
     calls the registerProducer() function of the CommodityInformation
     instance associated with the commodity
     @param commodity the commodity gaining a new producer
     @param producer the producer to be registered
   */
  void registerProducer(const Commodity& commodity, 
                        const Producer& producer);

  /**
     adds a player manager to the set of managers for a given 
     commodity
     @param commodity the commodity gaining a new player manager
     @param m the new player manager
   */
  void registerPlayerManager(const Commodity& commodity, 
                             MarketPlayerManager* m);

  /**
     the demand for a commodity at a given time
     @param commodity the commodity
     @param time the time
   */
  double demand(const Commodity& commodity, int time);

  /**
     returns the demand function for a commodity
     @param commodity the commodity being queried
   */
  FunctionPtr demandFunction(const Commodity& commodity);

  /**
     returns the current supply of a commodity
     @param commodity the commodity
     @return the current supply of the commodity
   */
  double supply(const Commodity& commodity);
  
  /**
     return the number of producers of a given commodity
     @param commodity the commodity
     @return the number of producers of a commodity
   */
  int nProducers(const Commodity& commodity);

  /**
     return a specific producer of a commodity
     @param commodity the commodity
     @param index the producer's index
     @return a pointer to the producer of a commodity at an index
   */
  Producer* producer(const Commodity& commodity, int index);
  
 private:
  /// a container of all commodities known to the manager
  std::map<Commodity,CommodityInformation,
    CommodityCompare> commodities_;

  /// a container of all player managers known to the manager
  std::map<Commodity,std::set<MarketPlayerManager*>,
    CommodityCompare> player_managers_;
};

#endif
