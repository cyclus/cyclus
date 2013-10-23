#include "builder.h"

#include "error.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Builder::Builder() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Builder::~Builder() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Builder::RegisterProducer(CommodityProducer* producer) {
  if (producers_.find(producer) != producers_.end()) {
    throw KeyError("A builder is trying to register a producer twice.");
  } else {
    producers_.insert(producer);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Builder::UnRegisterProducer(CommodityProducer* producer) {
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
std::set<CommodityProducer*>::iterator
Builder::BeginningProducer() {
  return producers_.begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<CommodityProducer*>::iterator
Builder::EndingProducer() {
  return producers_.end();
}

} // namespace cyclus
