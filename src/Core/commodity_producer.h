#ifndef COMMODITYPRODUCER_H
#define COMMODITYPRODUCER_H

#include <map>
#include <set>

#include "commodity.h"

namespace cyclus {
namespace supply_demand
{
  /**
     a container to hold information about a commodity
   */
  struct CommodityInformation
  {
    CommodityInformation();
    CommodityInformation(double a_capacity, double a_cost);
    double cost;
    double capacity;
  };

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
       @return the set of commodities produced by this producers
     */
    std::set<Commodity,CommodityCompare> ProducedCommodities();

    /**
       @return true if the commodity is produced by this entity
       @param commodity the commodity in question
     */
    bool ProducesCommodity(const Commodity& commodity);

    /**
       @return the production capacity for a commodity
       @param commodity the commodity in question
     */
    double ProductionCapacity(const Commodity& commodity);

    /**
       @return the cost to produce a commodity at a given capacity
       @param commodity the commodity in question
     */
    double ProductionCost(const Commodity& commodity);

    // protected: @MJGFlag - should be protected. revise when tests can
    // be found by classes in the Utility folder
    /**
       register a commodity as being produced by this object
       @param commodity the commodity being produced
     */
    void AddCommodity(const Commodity& commodity);

    /**
       set the production capacity for a given commodity
       @param commodity the commodity being produced
       @param capacity the production capacity
     */
    void SetCapacity(const Commodity& commodity, double capacity);

    /**
       set the production cost for a given commodity
       @param commodity the commodity being produced
       @param cost the production cost
     */
    void SetCost(const Commodity& commodity, double cost);

    /**
       register a commodity as being produced by this object and set
       its relevant info
       @param commodity the commodity being produced
       @param info the information describing the commodity
     */
    void AddCommodityWithInformation(const Commodity& commodity, 
                                     const CommodityInformation& info);
    /**
       add all commodities produced by a source
       @param source the original commodity producer
     */
    void CopyProducedCommoditiesFrom(CommodityProducer* source);
    
    /**
       checks if ProducesCommodity() is true. if it is false, an 
       error is thrown.
       @param commodity the commodity in question
     */
    void ThrowErrorIfCommodityNotProduced(const Commodity& commodity);
    
    /// a default production capacity
    double default_capacity_;

    /// a default production cost
    double default_cost_;

  private:
    /// a collection of commodities and their production capacities
    std::map<Commodity,CommodityInformation,CommodityCompare> produced_commodities_;

    //#include "commodity_producer_tests.h"
    //friend class CommodityProducerTests; 
    // @MJGFlag - removed for the same reason as above
  };
}
} // namespace cyclus
#endif
