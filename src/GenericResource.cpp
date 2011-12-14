// Resource.cpp
// Implements the Resource Class
#include "GenericResource.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
GenericResource::GenericResource(std::string units, std::string quality, double quantity)
  : Resource() {
    units_ = units;
    quality_ = quality;
    quantity_ = quantity;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
bool GenericResource::checkQuality(Resource* first, Resource* second){
  bool toRet = false;
  toRet = first->getResourceUnits() == second->getResourceUnits();
  return toRet;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool GenericResource::checkQuantityEqual(Resource* first, Resource* second) {
  // KDHFLAG : Should allow epsilon as a parameter to this function?
  bool toRet;
  toRet =( checkQuality(first,second) && first->getQuantity() == second->getQuantity());
  return toRet;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool GenericResource::checkQuantityGT(Resource* first, Resource* second) {
  // KDHFLAG : Should allow epsilon as a parameter to this function?
  bool toRet;
  toRet = ( checkQuality(first,second) && first->getQuantity() < second->getQuantity());
  return toRet;
}
