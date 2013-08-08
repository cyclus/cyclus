#include "builder.h"

#include "error.h"


//using namespace supply_demand;
//using namespace action_building;

namespace cyclus {
namespace action_building {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Builder::Builder() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Builder::~Builder() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Builder::RegisterProducer(supply_demand::CommodityProducer* producer) {
  if (producers_.find(producer) != producers_.end()) {
    throw KeyError("A builder is trying to register a producer twice.");
  } else {
    producers_.insert(producer);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Builder::UnRegisterProducer(supply_demand::CommodityProducer* producer) {
  if (producers_.find(producer) == producers_.end()) {
    throw KeyError("A builder is trying to unregister a producer not originally registered with it.");
  } else {
    producers_.erase(producer);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Builder::NBuildingPrototypes() {
  return producers_.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<supply_demand::CommodityProducer*>::iterator
Builder::BeginningProducer() {
  return producers_.begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<supply_demand::CommodityProducer*>::iterator
Builder::EndingProducer() {
  return producers_.end();
}
} // namespace ActionBuilder
} // namespace cyclus
