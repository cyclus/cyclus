#include "CommodityProducer.h"

#include "CycException.h"

using namespace std;
using namespace SupplyDemand;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityInformation::CommodityInformation() :
  capacity(0),
  cost(0) 
{}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityInformation::CommodityInformation(double a_capacity, 
                                           double a_cost) :
  capacity(a_capacity),
  cost(a_cost)
{}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityProducer::CommodityProducer() : 
  default_capacity_(0.0),
  default_cost_(0.0)
{}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CommodityProducer::~CommodityProducer() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool CommodityProducer::producesCommodity(Commodity& commodity)
{
  return (produced_commodities_.find(commodity) != 
          produced_commodities_.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducer::productionCapacity(Commodity& commodity)
{
  throwErrorIfCommodityNotProduced(commodity);
  return produced_commodities_[commodity].capacity;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CommodityProducer::productionCost(Commodity& commodity)
{
  throwErrorIfCommodityNotProduced(commodity);
  return produced_commodities_[commodity].cost;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::addCommodity(Commodity& commodity) {
  CommodityInformation info(default_capacity_,default_cost_);
  addCommodityWithInformation(commodity,info);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::setCapacity(Commodity& commodity, 
                                    double capacity)
{
  throwErrorIfCommodityNotProduced(commodity);
  produced_commodities_[commodity].capacity = capacity;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::setCost(Commodity& commodity, 
                                double cost)
{
  throwErrorIfCommodityNotProduced(commodity);
  produced_commodities_[commodity].cost = cost;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CommodityProducer::addCommodityWithInformation(Commodity& commodity, 
                                                    CommodityInformation& info) 
{
  if (producesCommodity(commodity))
    {
      throw CycDoubleRegistrationException("This producer already has registered "
                                           + commodity.name());
    }
  produced_commodities_.insert(make_pair(commodity,info));
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
