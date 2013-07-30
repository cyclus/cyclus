#include "Prototype.h"

#include "Model.h"
#include "CycException.h"
#include <utility>

namespace cyclus {

// init members
map<string,Prototype*> Prototype::prototype_registry_;

// -------------------------------------------------------------------
void Prototype::registerPrototype(std::string name, Prototype* p) {
  prototype_registry_.insert(make_pair(name,p));
}

// -------------------------------------------------------------------
Prototype* Prototype::getRegisteredPrototype(std::string name) {
  map<string,Prototype*>::iterator it = prototype_registry_.find(name);
  if (it == prototype_registry_.end()) {
    throw CycKeyException(name + "is not a registered prototype.");
  }
  return it->second;
}
} // namespace cyclus
