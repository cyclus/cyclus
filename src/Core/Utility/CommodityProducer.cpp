#include "CommodityProducer.h"

#include "cyclopts_limits.h"
#include "error.h"

namespace cyclus {
namespace SupplyDemand {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityInformation::CommodityInformation()
    : capacity(0),
      cost(cyclopts::kModifierLimit) 
{}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityInformation::CommodityInformation(double a_capacity, 
                                           double a_cost)
    : capacity(a_capacity),
      cost(a_cost)
{}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityProducer::CommodityProducer()
    : default_capacity_(0.0),
      default_cost_(cyclopts::kModifierLimit)
{}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityProducer::~CommodityProducer() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::set<Commodity,CommodityCompare> CommodityProducer::producedCommodities()
{
  std::set<Commodity,CommodityCompare> commodities;
  std::map<Commodity,CommodityInformation,CommodityCompare>::iterator it;
  for (it = produced_commodities_.begin(); 
       it != produced_commodities_.end(); 
       it++)
    {
      commodities.insert(it->first);
    }
  return commodities;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool CommodityProducer::producesCommodity(const Commodity& commodity)
{
  return (produced_commodities_.find(commodity) != 
          produced_commodities_.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducer::productionCapacity(const Commodity& commodity)
{
  throwErrorIfCommodityNotProduced(commodity);
  return produced_commodities_[commodity].capacity;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducer::productionCost(const Commodity& commodity)
{
  throwErrorIfCommodityNotProduced(commodity);
  return produced_commodities_[commodity].cost;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::addCommodity(const Commodity& commodity) {
  CommodityInformation info(default_capacity_,default_cost_);
  addCommodityWithInformation(commodity,info);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::setCapacity(const Commodity& commodity, 
                                    double capacity)
{
  throwErrorIfCommodityNotProduced(commodity);
  produced_commodities_[commodity].capacity = capacity;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::setCost(const Commodity& commodity, 
                                double cost)
{
  throwErrorIfCommodityNotProduced(commodity);
  produced_commodities_[commodity].cost = cost;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::addCommodityWithInformation(const Commodity& commodity, 
                                                    const CommodityInformation& info) 
{
  if (producesCommodity(commodity))
    {
      throw KeyError("This producer already has registered "
                                           + commodity.name());
    }
  produced_commodities_.insert(std::make_pair(commodity,info));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::copyProducedCommoditiesFrom(CommodityProducer* source) 
{
  std::set<Commodity,CommodityCompare> commodities = source->producedCommodities();
  std::set<Commodity,CommodityCompare>::iterator it;
  for (it = commodities.begin(); it != commodities.end(); it++)
    {
      addCommodity(*it);
      setCapacity(*it,source->productionCapacity(*it));
      setCost(*it,source->productionCost(*it));
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::throwErrorIfCommodityNotProduced(const Commodity& commodity)
{
  if(!producesCommodity(commodity))
    {
      throw KeyError("Producer does not produce " 
                                      + commodity.name());
    }
}
} // namespace SupplyDemand
} // namespace cyclus
