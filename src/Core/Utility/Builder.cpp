#include "Builder.h"

#include "CycException.h"


//using namespace SupplyDemand;
//using namespace ActionBuilding;

namespace cyclus {
namespace ActionBuilding {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Builder::Builder() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Builder::~Builder() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Builder::registerProducer(SupplyDemand::CommodityProducer* producer) {
  if (producers_.find(producer) != producers_.end())
    {
      throw CycDoubleRegistrationException("A builder is trying to register a producer twice.");
    }
  else
    {
      producers_.insert(producer);
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Builder::unRegisterProducer(SupplyDemand::CommodityProducer* producer) {
  if (producers_.find(producer) == producers_.end())
    {
      throw CycNotRegisteredException("A builder is trying to unregister a producer not originally registered with it.");
    }
  else
    {
      producers_.erase(producer);
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double Builder::nBuildingPrototypes() 
{
  return producers_.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::set<SupplyDemand::CommodityProducer*>::iterator Builder::beginningProducer()
{
  return producers_.begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::set<SupplyDemand::CommodityProducer*>::iterator Builder::endingProducer()
{
  return producers_.end();
}
} // namespace ActionBuilder
} // namespace cyclus
