#include "builder.h"

#include "error.h"

namespace cyclus {
namespace toolkit {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Builder::Builder() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Builder::~Builder() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Builder::RegisterProducer(CommodityProducer* producer) {
  if (commod_producers_.find(producer) != commod_producers_.end()) {
    throw KeyError("A builder is trying to register a producer twice.");
  } else {
    commod_producers_.insert(producer);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Builder::UnRegisterProducer(CommodityProducer* producer) {
  if (commod_producers_.find(producer) == commod_producers_.end()) {
    throw KeyError("A builder is trying to unregister"
                   " a producer not originally registered with it.");
  } else {
    commod_producers_.erase(producer);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Builder::NBuildingPrototypes() {
  return commod_producers_.size();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<CommodityProducer*>::iterator
Builder::BeginningProducer() {
  return commod_producers_.begin();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<CommodityProducer*>::iterator
Builder::EndingProducer() {
  return commod_producers_.end();
}

} // namespace toolkit
} // namespace cyclus
