#include "Prototype.h"

#include "Model.h"
#include "error.h"
#include <utility>

namespace cyclus {

// init members
std::map<std::string, Prototype*> Prototype::prototype_registry_;

// -------------------------------------------------------------------
void Prototype::registerPrototype(std::string name, Prototype* p) {
  prototype_registry_.insert(std::make_pair(name,p));
}

// -------------------------------------------------------------------
Prototype* Prototype::getRegisteredPrototype(std::string name) {
  std::map<std::string, Prototype*>::iterator it = prototype_registry_.find(name);
  if (it == prototype_registry_.end()) {
    throw KeyError(name + "is not a registered prototype.");
  }
  return it->second;
}
} // namespace cyclus
