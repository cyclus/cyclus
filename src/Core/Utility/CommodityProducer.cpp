#include "CommodityProducer.h"

#include "CycException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityProducer::CommodityProducer() : default_capacity_(0.0) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityProducer::~CommodityProducer() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducer::productionCapacity(Commodity& commodity)
{
  throwErrorIfCommodityNotProduced(commodity);
  return production_capacities_[commodity];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool CommodityProducer::producesCommodity(Commodity& commodity)
{
  return (production_capacities_.find(commodity) != 
          production_capacities_.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::addCommodity(Commodity& commodity) {
  if (producesCommodity(commodity))
    {
      throw CycDoubleRegistrationException("This producer already has registered "
                                           + commodity.name());
    }

  production_capacities_.insert(make_pair(commodity,default_capacity_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::setCapacity(Commodity& commodity, 
                                    double capacity)
{
  throwErrorIfCommodityNotProduced(commodity);
  production_capacities_[commodity] = capacity;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::addCommodityAndSetCapacity(Commodity& commodity, 
                                                   double capacity) 
{
  addCommodity(commodity);
  setCapacity(commodity,capacity);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::throwErrorIfCommodityNotProduced(Commodity& commodity)
{
  if(!producesCommodity(commodity))
    {
      throw CycNotRegisteredException("Producer does not produce " 
                                      + commodity.name());
    }
}
