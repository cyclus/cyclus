#ifndef SUPPLYDEMANDMANAGER_H
#define SUPPLYDEMANDMANAGER_H

#include "SupplyDemand.h"
#include "SymbolicFunctions.h"

#include <vector>
#include <map>

/**
   This is a manager class that manages a set of commoditys. Those commoditys have a certain
   demand function associated with them and a list of producers who can produce the
   commoditys.

   The SupplyDemandManager simply keeps track of this information and provides the
   demand and supply of a commodity at a given time. What to do with this information is 
   left to the user of the SupplyDemandManager.
 */
class SupplyDemandManager {
 public:
  /**
     constructor
   */
  SupplyDemandManager();

  /**
     register a new commodity with the manager, along with all the necessary
     information
     @param commodity the commodity
     @param fp a smart pointer to the demand function
     @param producers the list of producers of commodity
   */
  void registerCommodity(const Commodity& commodity, const FunctionPtr fp, 
                       const std::vector<Producer>& producers);

  /**
     calls the registerProducer() function of the CommodityInformation
     instance associated with the commodity
     @param the commodity gaining a new producer
     @param the producer to be registered
   */
  void registerProducer(const Commodity& commodity, const Producer& producer);

  /**
     the demand for a commodity at a given time
     @param commodity the commodity
     @param time the time
   */
  double demand(const Commodity& commodity, int time);

  /**
     returns the current supply of commodity p
     @param commodity the commodity
     @return the current supply of the commodity
   */
  double supply(const Commodity& commodity);

  /**
     increase the supply of a given commodity by an amount
     @param commodity the commodity
     @param amt the amount to increase
   */
  void increaseSupply(const Commodity& commodity, double amt);
  
  /**
     return the number of producers of a given commodity
     @param commodity the commodity
     @return the number of producers of commodity p
   */
  int nProducers(const Commodity& commodity);

  /**
     return a specific producer of a commodity
     @param commodity the commodity
     @param index the producer's index
     @return a pointer to the producer of commodity p at index
   */
  Producer* producer(const Commodity& commodity, int index);
  
 private:
  /// a container of all commoditys known to the manager
  std::map<Commodity,CommodityInformation,CommodityCompare> commodities_;
};

#endif
