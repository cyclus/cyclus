#ifndef COMMODITYPRODUCER_H
#define COMMODITYPRODUCER_H

#include <map>

#include "Commodity.h"

namespace SupplyDemand
{
  /**
     a mixin to provide information about produced commodities
   */
  class CommodityProducer
  {
  public:
    /// constructor
    CommodityProducer();

    /// virtual destructor for inheritence
    virtual ~CommodityProducer();

    /**
       @return the production capacity for a commodity
       @param commodity the commodity in question
     */
    double productionCapacity(Commodity& commodity);

    /**
       @return true if the commodity is produced by this entity
       @param commodity the commodity in question
     */
    bool producesCommodity(Commodity& commodity);

    // protected: @MJGFlag - should be protected. revise when tests can
    // be found by classes in the Utility folder
    /**
       register a commodity as being produced by this object
       @param commodity the commodity being produced
     */
    void addCommodity(Commodity& commodity);

    /**
       set the production capacity for a given commodity
       @param commdity the commodity being produced
       @param capacity the production capacity
     */
    void setCapacity(Commodity& commodity, double capacity);

    /**
       register a commodity as being produced by this object and set
       its production capacity
       @param commodity the commodity being produced
       @param capacity the production capacity
     */
    void addCommodityAndSetCapacity(Commodity& commodity, 
                                    double capacity);
    
    /**
       checks if producesCommodity() is true. if it is false, an 
       error is thrown.
       @param commodity the commodity in question
     */
    void throwErrorIfCommodityNotProduced(Commodity& commodity);
    
    /// a default production capacity
    double default_capacity_;

  private:
    /// a collection of commodities and their production capacities
    std::map<Commodity,double,CommodityCompare> production_capacities_;

    //#include "CommodityProducerTests.h"
    //friend class CommodityProducerTests; 
    // @MJGFlag - removed for the same reason as above
  };
}

#endif
