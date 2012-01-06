// Resource.cpp
// Implements the Resource Class
#include "GenericResource.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
GenericResource::GenericResource(std::string units,
            std::string quality, double quantity) : Resource() {
    units_ = units;
    quality_ = quality;
    quantity_ = quantity;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
GenericResource* GenericResource::clone() {
  return new GenericResource(*this);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
bool GenericResource::checkQuality(Resource* other){
  bool toRet = false;
  toRet = (units_== other->getResourceUnits());
  return toRet;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool GenericResource::checkQuantityEqual(Resource* other) {
  // KDHFLAG : Should allow epsilon as a parameter to this function?
  bool toRet;
  toRet =( checkQuality(other) && quantity_ == other->getQuantity());
  return toRet;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool GenericResource::checkQuantityGT(Resource* other) {
  // KDHFLAG : Should allow epsilon as a parameter to this function?
  bool toRet;
  toRet = ( checkQuality(other) && quantity_ < other->getQuantity());
  return toRet;
}
