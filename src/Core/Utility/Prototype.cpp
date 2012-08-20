#include "Prototype.h"

#include "Model.h"
#include "CycException.h"
#include <utility>

using namespace std;

// init members
map<string,Prototype*> Prototype::prototype_registry_;

// -------------------------------------------------------------------
Prototype::registerPrototype(std::string name, Prototype* p) {
  prototype_registry_.insert(make_pair(name,p));
}

// -------------------------------------------------------------------
Prototype* Prototype::getRegisteredPrototype(std::string name) {
  map<string,Prototype*>::iterator it = prototype_registry_.find(name);
  if (it == prototype_registry_.end()) {
    throw CycKeyException(name + "is not a registered prototype.");
  }
  return *it;
}

// -------------------------------------------------------------------
Prototype* Prototype::clone(std::string name) {
  Prototype* prototype = getRegisteredPrototype(name);
  Prototype* clone = copyInitializedMembers(prototype);
  return clone;
}

